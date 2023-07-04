// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_SolaseadoPhoton.h"
#include "GameModeBase_Solaseado.h"
#include "Actor_PhotonChat.h"

#include "Engine/LevelStreaming.h"

#include <time.h>

#define Photon_AppID "6b83afef-1da9-4b42-9281-da551443a54b"
#define Photon_ServerAddress "ns.exitgames.com"

//1000ms=1second 움직일 때 마다 업데이트(임시 1초)
#define Update_Move_Time 1000

// Sets default values
AActor_SolaseadoPhoton::AActor_SolaseadoPhoton() : serverAddress(TEXT(Photon_ServerAddress)), AppID(TEXT(Photon_AppID))
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<APawn_Player> FindtargetCharacter(TEXT("Blueprint'/Game/Project/Pawn/Solaseado/BP_OtherPlayer.BP_OtherPlayer_C'"));
	//FindtargetCharacter(TEXT("/Game/Project/BP_targetPlayer")); 
	if (FindtargetCharacter.Succeeded())
		targetCharacter = FindtargetCharacter.Class;

	//플레이어 스피드 임시 처리
	moveSpeed = 500.0f;
	//이 거리 이상 멀어지면 강제 위치 보정을 해준다.
	lerpDistance = 200.0f;
}

AActor_SolaseadoPhoton::~AActor_SolaseadoPhoton()
{
	if (m_pListener)
	{
		delete m_pListener;
		delete m_pClient;
	}
	if (dummy_pListener)
	{
		delete dummy_pClient;
		delete dummy_pListener;
	}
}

// Called when the game starts or when spawned
void AActor_SolaseadoPhoton::BeginPlay()
{
	Super::BeginPlay();

	this->SetTickableWhenPaused(true);

	const UWorld* world = GetWorld();
	if (world)
	{
		LocalPlayer = Cast<APawn_Player>(world->GetFirstPlayerController()->GetPawn());
	}
}

void AActor_SolaseadoPhoton::movePlayer(FVector Loc)
{
	if (m_pListener)
	{
		m_pListener->SetMovePlayer(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z);
	}
}

void AActor_SolaseadoPhoton::movePlayer(int vx, int vy, int vz)
{
	if (m_pListener)
	{
		m_pListener->SetMovePlayer(vx, vy, vz);
	}
}

void AActor_SolaseadoPhoton::movePlayerRotation(float fZ)
{
	if (m_pListener)
	{
		m_pListener->SetMovePlayerRotation(fZ);
	}
}


// Called every frame
void AActor_SolaseadoPhoton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	unsigned long t = GETTIMEMS();
	if ((t - lastUpdateTime) > 100)
	{
		lastUpdateTime = t;
		if (m_pListener)
		{
			m_pListener->service();
		}
		if (dummy_pListener)
		{
			dummy_pListener->service();
		}
	}

	//아직 테스트 중인 이동 동기화 부분을 bOnTimeMove로 On/Off한다
	//총 움직인 시간이 Update_Move_Time(1초)을 넘을 때 마다 위치 업데이트
	if (bOnTimeMove && bIsMoving && lastMoveTime+t-startMoveTime> Update_Move_Time)
	{
		lastMoveTime = 0;
		startMoveTime = t;
		
		//회전
		movePlayerRotation(LocalPlayer->GetActorRotation().Yaw);
		//위치
		MovePlayerAndTime(LocalPlayer->GetActorLocation().X, LocalPlayer->GetActorLocation().Y, m_pClient->getServerTime() % 10000);
	}
}



void AActor_SolaseadoPhoton::ConnectLogin(const FString& username)
{
	//포톤 채팅 connect
	ConnectPhotonCHat(); // 블루프린트 스폰시 photonchat변수담고 // JoinOrCreateComplete 때 방진입

	srand(GETTIMEMS());
	m_pListener = new PhotonListner_Solaseado(this);
	m_pClient = new ExitGames::LoadBalancing::Client(*m_pListener, TCHAR_TO_UTF8(*AppID), TCHAR_TO_UTF8(*appVersion),
		Photon::ConnectionProtocol::DEFAULT, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT, false); //  (nByte)0U, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT
	m_pListener->SetClient(m_pClient);
	m_pListener->Connect(TCHAR_TO_UTF8(*username), TCHAR_TO_UTF8(*serverAddress));
	
	
	//더미생성만
	dummy_pListener = new PhotonListner_Solaseado(this);
	dummy_pClient = new ExitGames::LoadBalancing::Client(*dummy_pListener, TCHAR_TO_UTF8(*AppID), TCHAR_TO_UTF8(*appVersion),
		Photon::ConnectionProtocol::DEFAULT, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT, false); //  (nByte)0U, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT
	//세팅
	dummy_pListener->setDummy(true);
	dummy_pListener->SetClient(dummy_pClient);
}

void AActor_SolaseadoPhoton::ChangeRoom(int Number)
{
	if (!IsChangingRoom)
	{
		IsChangingRoom = true; //  == > JoinOrCreateComplete  에서 룸접속 후 false로 바꿈 // 중복클릭 방지
		m_pListener->ChangeRoomNumber(Number);
		m_pClient->opLeaveRoom(); // ==> LeaveRoomComplete
	}
}

///////////////////////////////////////////////////////////////////////
///////////////////Dummy or RoomSetting 시작
void AActor_SolaseadoPhoton::OpenDummy()
{
	FString FuserName = m_pClient->getLocalPlayer().getName().UTF8Representation().cstr();
	FString DummyName = "dummy_" + FuserName;
	//접속
	dummy_pListener->Connect(TCHAR_TO_UTF8(*DummyName), TCHAR_TO_UTF8(*serverAddress));
}

void AActor_SolaseadoPhoton::UpdateRoomList(const TMap<int, int>& Channel_Count)
{
	ChannelList = Channel_Count;
	ChangeViewCount(Channel_Count);
}

void AActor_SolaseadoPhoton::CloseDummy()
{
	dummy_pClient->disconnect();
}
////////////////////////////////////////////////Dummy or Room Setting끝


// 텍스트 메세지 출력 
void AActor_SolaseadoPhoton::SendTextMessage(const FString& Message, const FString& type)
{
	if (m_pListener)
		m_pListener->TextMessage(TCHAR_TO_UTF8(*Message), TCHAR_TO_UTF8(*type));
}


//커맨드를 입력했을때 이 함수를 써서 포톤 서버에 뿌려준다.
//이동은 서버에서 받았을때 처리한다.
//이전에 입력이 됐는지에 대한 검사는 다른 함수로 처리하고 입력받는다.
void AActor_SolaseadoPhoton::movePlayerCommand(enum_InputPlayer _Commnad)
{
	if (m_pListener)
	{
		m_pListener->SetPlayerCommand((int)_Commnad);
	}
}

//
void AActor_SolaseadoPhoton::RotationPlayerX(float fX)
{
	if (m_pListener)
	{
		m_pListener->SetPlayerRotationCommand(fX);
	}
}

//현재 사용하지 않음
void AActor_SolaseadoPhoton::MovePlayerRotationAndTime(float fX, int time)
{

}

void AActor_SolaseadoPhoton::MovePlayerAndTime(int vX, int vY, int time)
{
	if (m_pListener)
	{
		m_pListener->SetPlayerMoveAndTime(vX,vY,time);
	}
}

void AActor_SolaseadoPhoton::MoveAndRotation(FVector Loc, int vYaw)
{
	if (m_pListener)
	{
		m_pListener->SetMoveAndRotation(Loc.X, Loc.Y, Loc.Z, vYaw);
	}
}

void AActor_SolaseadoPhoton::ReconnectMessage()
{
	this->Destroy();
}

void AActor_SolaseadoPhoton::PhotonDisconnect()
{
	if (m_pClient)
		m_pClient->disconnect();
}

void AActor_SolaseadoPhoton::setRegion(ExitGames::LoadBalancing::Client* Client)
{
	if (Client)
	{
		Client->selectRegion("kr");
	}
}

void AActor_SolaseadoPhoton::ErrorCheckMessage(const FString& message, int error)
{
	//UE_LOG(LogTemp, Log, TEXT("// ErrorCheckMessage :: %s"), *message);
	//UE_LOG(LogTemp, Log, TEXT("// ErrorCode :: %d"), error);
}

//필드 초기화
void AActor_SolaseadoPhoton::InitPlayers(void)
{
	for (auto it : PlayerList)
	{
		if (!it->bLocal)
		{
			it->Destroy();
		}
	}
	PlayerList.Reset();
	
}

void AActor_SolaseadoPhoton::AddPlayers(int playerNr, const ExitGames::Common::JString& playerName, bool local, const ExitGames::Common::Hashtable& Custom)
{
	FString name = FString(UTF8_TO_TCHAR(playerName.UTF8Representation().cstr()));

	//// 추가 커스텀 데이터 >> 애니메이션 추가
	//int size = Custom.getSize();

	//입력받은 코스튬 세팅
	TArray<FString> ArrayPlayerCostume;
	for (int i = 0; i < DataCount; ++i)
	{
		FString str = "c"; // Costume Data
		str += FString::FromInt(i);

		//이전코드
		//char* Temp = TCHAR_TO_UTF8(*str);

		//수정된 코드
		const TCHAR* TempTCHAR = *str;
		const char* Temp = new char[FCString::Strlen(TempTCHAR) + 1];
		FCStringAnsi::Strcpy(const_cast<char*>(Temp), FCString::Strlen(TempTCHAR) + 1, TCHAR_TO_UTF8(TempTCHAR));

		if (Custom.contains(Temp))
		{
			JString Costume = ((ValueObject<JString>*)Custom.getValue(Temp))->getDataCopy();
			ArrayPlayerCostume.Add(FString(UTF8_TO_TCHAR(Costume.UTF8Representation().cstr())));
		}

		FMemory::Free((void*)Temp);
	}


	if (local)
	{
		// UE_LOG(LogTemp, Log, TEXT("// Add Local Player :: %s"), *name);
		PlayerList.Add(LocalPlayer);
		LocalPlayer->AddClentPlayerCount();
		if (LocalPlayer)
		{
			LocalPlayer->isInRoom = true;
			LocalPlayer->isInLoby = false;
			LocalPlayer->PlayerNr = playerNr;
			LocalPlayer->bLocal = true;
			LocalPlayer->PlayerName = name;
			//LocalPlayer->AddMainWidget();
			//LocalPlayer->setPlayerNameTag(name);

			//포톤으로 받은 코스튬 데이터를 전달한다.
			LocalPlayer->SetCostumeArray(ArrayPlayerCostume);

		}
	}
	else
	{
		FActorSpawnParameters actorSpawnParam;
		actorSpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APawn_Player* target = GetWorld()->SpawnActor<APawn_Player>(targetCharacter, FTransform(), actorSpawnParam);
		//target->SpawnDefaultController();//이게 없어서 무브먼트가 안먹혔음, 블루프린트에서 auto possess ai로 설정 가능
		if (target)
		{
			// 타겟 플레이어 이름
			target->PlayerName = name;
			target->PlayerNr = playerNr;
			//target->setPlayerNameTag(name);

			// 애니메이션 데이터 
			// FString str = FString(UTF8_TO_TCHAR(chAnim.UTF8Representation().cstr()));
			// target->bWalk = (bool)FCString::Atoi(*str);

			PlayerList.Add(target);
			LocalPlayer->AddClentPlayerCount();

			//UE_LOG(LogTemp, Log, TEXT("// addplayer yaw :: %d , local : %d"), playerNr, LocalPlayer->PlayerNr);
			MoveAndRotation(LocalPlayer->GetActorLocation(), LocalPlayer->GetActorRotation().Yaw);

			//포톤으로 받은 코스튬 데이터를 전달한다.
			target->SetCostumeArray(ArrayPlayerCostume);
		}
	}
}

//접속중인 플레이어 제거
void AActor_SolaseadoPhoton::RemovePlayer(int playerNr)
{
	// 참여자 리스트 위젯 
	// RemovePlayerEvent.Broadcast(playerNr);
	for (auto it : PlayerList)
	{
		// 참여자 액터
		if (it->PlayerNr == playerNr)
		{
			// UE_LOG(LogTemp, Log, TEXT("// RemovePlayer :: playerNr( %d )"), playerNr);
			LocalPlayer->RemoveClentPlayerCount();
			it->Destroy();
			PlayerList.Remove(it);
			return;
		}
	}
}

/****************************************
	서버에서 받아온 다른 플레이어들의 정보 갱신 구간
	*****************************************/
void AActor_SolaseadoPhoton::updatePlayerProperties(int playerNr, const Hashtable& changes)
{

	// 캐릭터 애니메이션 변경 적용 
	if (changes.contains("An"))
	{
		uint8 Anim = ((ValueObject<uint8>*)changes.getValue("An"))->getDataCopy();
		for (auto it : PlayerList)
		{
			if (it->PlayerNr == playerNr)
			{
				it->eAnimationState = (enum_PlayerAnimationState)Anim;
				//UE_LOG(LogTemp, Log, TEXT("// Change Anim Player :: %d, State :: %d"), playerNr, Anim);
				break;
			}
		}
	}

	
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			//코스튬 구간(n=DataCount) ::  "C_0" ~ "C_n" 
			{
				TArray<FString> CostumeList;
				for (int i = 0; i < DataCount; i++)
				{
					FString str = "c"; // Costume Data
					str += FString::FromInt(i);

					const TCHAR* TempTCHAR = *str;
					const char* Temp = new char[FCString::Strlen(TempTCHAR) + 1];
					FCStringAnsi::Strcpy(const_cast<char*>(Temp), FCString::Strlen(TempTCHAR) + 1, TCHAR_TO_UTF8(TempTCHAR));

					JString Costume = ""; // 변경 코스튬

					if (changes.contains(Temp))
					{
						Costume = ((ValueObject<JString>*)changes.getValue(Temp))->getDataCopy();
						FString cc = FString(UTF8_TO_TCHAR(Costume.UTF8Representation().cstr()));
						CostumeList.Add(cc);
					}

					FMemory::Free((void*)Temp);
				}
				it->SetCostumeArray(CostumeList);
			}

			//개별 코스튬 파츠 업데이트 구간
			{
				TArray<FString> CostumeList;

				FString str = "costume"; // Costume Data

				const TCHAR* TempTCHAR = *str;
				const char* Temp = new char[FCString::Strlen(TempTCHAR) + 1];
				FCStringAnsi::Strcpy(const_cast<char*>(Temp), FCString::Strlen(TempTCHAR) + 1, TCHAR_TO_UTF8(TempTCHAR));

				JString Costume = ""; // 변경 코스튬

				if (changes.contains(Temp))
				{
					Costume = ((ValueObject<JString>*)changes.getValue(Temp))->getDataCopy();
					FString cc = FString(UTF8_TO_TCHAR(Costume.UTF8Representation().cstr()));
					CostumeList.Add(cc);
				}

				FMemory::Free((void*)Temp);
				it->SetCostumeArray(CostumeList);
			}
			break;
		}
	}


}


void AActor_SolaseadoPhoton::setPlayerAnimationData(enum_PlayerAnimationState anim)
{
	if (m_pListener)
		m_pListener->SendPlayerAnimState((uint8)anim);
}



//캐릭터 움직임
void AActor_SolaseadoPhoton::GetMovePlayer(int playerNr, int vX, int vY, int vZ)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			// 높이값 고정 으로 변경 vZ
			it->SetActorLocation(FVector(vX, vY, PlayerHeight));
			return;
		}
	}
}

void AActor_SolaseadoPhoton::GetMovePlayerRotation(int playerNr, float fX)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			it->SetActorRotation(FRotator(0, fX, 0));
			return;
		}
	}
}


void AActor_SolaseadoPhoton::GetPlayerRotationYaw(int playerNr, float fYaw)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			it->fRotationX = fYaw;

			return;
		}
	}
}


void AActor_SolaseadoPhoton::GetMoveAndRotation(int playerNr, int vX, int vY, int vZ, int vYaw)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			it->SetActorLocation(FVector(vX, vY, vZ));
			it->SetActorRotation(FRotator(0, vYaw, 0));
			return;
		}
	}
}


//서버에서 플레어어들의 입력 데이터를 입력받는다.
//처리는 기존과 같이 일단 Tick에서 처리한다.
void AActor_SolaseadoPhoton::GetMovePlayerCommand(int playerNr, int iCommand)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			it->PhotonTimer = GETTIMEMS() % 10000;
			//딜레이 테스트용(테스트 끝나면 필요없음)
			//m_pListener->SendTestDelay(it->PhotonTimer);

			//아직 테스트 중인 이동 동기화 부분을 bOnTimeMove로 On/Off한다
			if (it->InputMoveCommand((enum_InputPlayer)iCommand) && playerNr == LocalPlayer->PlayerNr && bOnTimeMove)
			{
				//움직임 업데이트 체크
				//
				if ((it->fForward || it->fRight) && !bIsMoving)
				{
					// 움직임 업데이트 시작
					// 이제 막 움직였을 경우 움직인 시간을 측정하기 위해 현재 시간을 저장한다.
					bIsMoving = true;
					startMoveTime = GETTIMEMS();
					InputAnimationState(enum_PlayerAnimationState::Walk);
					

					//UE_LOG(LogTemp, Log, TEXT("//Start Walk x== %f, y== %f"), LocalPlayer->fForward, LocalPlayer->fRight);
					//UE_LOG(LogTemp, Log, TEXT("//Check Moving Time"));
				}
				else if ((it->fForward == 0 && it->fRight == 0) && bIsMoving)
				{
					// 움직임 업데이트 종료
					// 움직인 시간을 다음에 처리하기 위해 lastMoveTime에 저장한다.
					bIsMoving = false;
					lastMoveTime = GETTIMEMS() - startMoveTime;
					InputAnimationState(enum_PlayerAnimationState::Idle);

					//UE_LOG(LogTemp, Log, TEXT("//End Walk x== %f, y== %f"), LocalPlayer->fForward, LocalPlayer->fRight);
				}
			}
			return;
		}
	}
}

//현재 사용중이지 않음
void AActor_SolaseadoPhoton::GetMovePlayerRotationAndTime(int playerNr, float fX, int time)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			
		}
	}
}

void AActor_SolaseadoPhoton::GetMovePlayerAndTime(int playerNr, int vX, int vY, int time)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			float tTime = (float)time * 0.001f * moveSpeed;

			//현재 위치 + 왕복시간으로 예측한 이동 위치
			FVector lerpVector = FVector(vX, vY, PlayerHeight)+ (it->GetActorForwardVector() * it->fForward + it->GetActorRightVector() * it->fRight).GetSafeNormal() * tTime;
			
			//UE_LOG(LogTemp, Log, TEXT("//lerp x== %f, y== %f, tTime==%f"), lerpVector.X, lerpVector.Y, tTime);
			//UE_LOG(LogTemp, Log, TEXT("//fForward== %f, fRight== %f"), it->fForward, it->fRight);
			
			//플레이어 간의 거리가 2m(lerpDistance) 이상 차이가 나면 위치값을 보정해준다.
			//UE_LOG(LogTemp, Log, TEXT("//Player NR %d Recv %d Distance== %f"), playerNr,LocalPlayer->PlayerNr,FVector::Distance(it->GetActorLocation(),  lerpVector));
			if (FVector::Distance(it->GetActorLocation(), lerpVector) > lerpDistance)
			{
				it->fLerpMoveX = (lerpVector.X - it->GetActorLocation().X) / lerpTimer;
				it->fLerpMoveY = (lerpVector.Y - it->GetActorLocation().Y) / lerpTimer;
				it->fLerpMoveZ = (lerpVector.Z - it->GetActorLocation().Z) / lerpTimer;
				it->lerpMoveTimer = lerpTimer;
			}
		}
	}
}

void AActor_SolaseadoPhoton::GetPlayerAnim(int playerNr, int Anim)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			it->eAnimationState = (enum_PlayerAnimationState)Anim;
			//UE_LOG(LogTemp, Log, TEXT("// GetPlayerAnim :: %d"), Anim);

		}
	}
}

void AActor_SolaseadoPhoton::ConnectComplete(void)
{
	//UE_LOG(LogTemp, Log, TEXT("// ConnectComplete "));
	
	InitPlayerData_Implementation();
}

void AActor_SolaseadoPhoton::CreateChannelComplete(const ExitGames::Common::JString& map, const ExitGames::Common::JString& channel)
{

}

void AActor_SolaseadoPhoton::CreateRoomComplete(const ExitGames::Common::JString& map)
{
	
}

void AActor_SolaseadoPhoton::JoinRoomComplete(const ExitGames::Common::JString& map, const ExitGames::Common::JString& channel)
{
	
}

// 룸에 들어왔을때 호출
void AActor_SolaseadoPhoton::JoinOrCreateComplete(const FString& RoomFullName)
{
	//UE_LOG(LogTemp, Log, TEXT("// JoinOrCreateComplete :: "));
	// #include "Actor_RosActor.h"	
	// 헤더 연결 오류 처리 // Blueprint Spawn 변경 
	//ConnectRosActor();



	if (PhotonChat) // 방에 완전히 진입했을때 채팅방 세팅
	{
		// 접속시키고나서 메인클라우드가 접속이 완료되면 룸번호를 얻고 채팅서버에 subscripe으로 넘겨주기
		PhotonChat->Chat_ResetJoinChannel(RoomFullName);
	}

	AGameModeBase_Solaseado* GM_Solaseado = Cast<AGameModeBase_Solaseado>(GetWorld()->GetAuthGameMode()); // moveLevel_ SimPoly1, Fade
	GM_Solaseado->MoveLevel(enum_Level::SimPoly1, true);
	IsChangingRoom = false;
}
// 룸 나갈때 호출
void AActor_SolaseadoPhoton::LeaveRoomComplete(void)
{
	if (LocalPlayer)
	{
		LocalPlayer->isInRoom = false;
		LocalPlayer->isInLoby = true;
	}
	if (IsChangingRoom)
	{
		InitPlayerData();
		//UE_LOG(LogTemp, Log, TEXT("// LeaveRoomComplete :: "));
	}
}

void AActor_SolaseadoPhoton::CurrentRoomInfo(const ExitGames::Common::JString& name, nByte Count, nByte MaxCount)
{
	CurrentRoomName = UTF8_TO_TCHAR(name.UTF8Representation().cstr());
	CurrentPeople = Count - 1;
	MaxPeople = MaxCount - 1;

	//UE_LOG(LogTemp, Log, TEXT("// RoomName :: %s"), *CurrentRoomName);
	//UE_LOG(LogTemp, Log, TEXT("// MaxPeople :: %d , CurrentPeople:: %d "), MaxCount, Count);
}

void AActor_SolaseadoPhoton::getTextMessage(int playerNr, const ExitGames::Common::JString& Message, const ExitGames::Common::JString& Type)
{
	FString TextMessage = FString(UTF8_TO_TCHAR(Message.UTF8Representation().cstr()));
	FString TextType = FString(UTF8_TO_TCHAR(Type.UTF8Representation().cstr()));

	// UE_LOG(LogTemp, Log, TEXT("// getTextMessage (%d) :: %s (%s)"), playerNr, *TextMessage, *TextType);
	// 
	for (auto it : PlayerList)
	{
		// UE_LOG(LogTemp, Log, TEXT("// getTextMessage :: %d "), it->PlayerNr);
		if (it->PlayerNr == playerNr)
		{
			Enum_TextType3 type = (Enum_TextType3)FCString::Atoi(*TextType);
			switch (type)
			{
			case Enum_TextType3::Normal:
			{
				// 말풍선 띄우기
				//it->OpenMessageBox(TextMessage);
				//LocalPlayer->AddChatList(it->PlayerName, TextMessage);
				break;
			}
			case Enum_TextType3::World:
			{
				// 화면 전체 메세지 띄우기
				//LocalPlayer->OpenWorldMessageBox(TextMessage, true);
				break;
			}
			case Enum_TextType3::infinite:
			{
				//LocalPlayer->OpenWorldMessageBox(TextMessage, false);
				break;
			}
			}
		}
	}
}


// photon : 룸 커스텀 데이터 변경 호출
void AActor_SolaseadoPhoton::ChangeRoomEventProperty(uint8 Ev)
{
	if (m_pListener)
		m_pListener->setRoomEventProperties(Ev);
}

// 룸 커스텀 데이터 변경 콜벡
void AActor_SolaseadoPhoton::updateRoomProperties(const Hashtable& changes)
{
	// 
	if (changes.contains("Ev"))
	{
		nByte roomEvent = ((ValueObject<nByte>*)changes.getValue("Ev"))->getDataCopy();
		//LocalPlayer->ChangeRoomEvent((uint8)roomEvent);
	}
}

// 이벤트 일시정지 호출
void AActor_SolaseadoPhoton::ChangeEventPause(bool ev)
{
	//if (m_pListener)
	//	m_pListener->setEventPause(ev);
}

// 이벤트 일시정지
void AActor_SolaseadoPhoton::getEventPause(bool ev)
{
	//if (LocalPlayer)
	//	LocalPlayer->ChangeEventPause(ev);
}

// 캐릭터 데이터 저장
// 
void AActor_SolaseadoPhoton::InputCharacterInfo(FString _key, FString _value)
{
	//UE_LOG(LogTemp, Log, TEXT("// InputCharacterInfo "));
	m_pListener->SetChracterInfo(_key, _value);
}

// 저장한 캐릭터 데이터 보내기
// 설명 보충 추가
void AActor_SolaseadoPhoton::SendPlayerInfo()
{
	//UE_LOG(LogTemp, Log, TEXT("// SendPlayerInfo "));
	m_pListener->SendCharacterInfo();
}


//로컬 플레이어 위치 갱신
void AActor_SolaseadoPhoton::updateLocalPlayerPosion()
{
	if (LocalPlayer && LocalPlayer->isInRoom)
	{
		//UE_LOG(LogTemp, Log, TEXT("// updateLocalPlayerPosion yaw: %f , x=%f ,y=%f ,z=%f "), LocalPlayer->GetActorRotation().Yaw, LocalPlayer->GetActorLocation().X, LocalPlayer->GetActorLocation().Y, LocalPlayer->GetActorLocation().Z);
		MoveAndRotation(LocalPlayer->GetActorLocation(), LocalPlayer->GetActorRotation().Yaw);
	}
}


//상태 애니메이션을 서버에 갱신하기
/*
Play_Pawn에서 관리중인 애니메이션 상태값이 바뀌었을때 여기에 넣어주면 다른 플레이어에게 상태값이 전달됩니다.
*/
void AActor_SolaseadoPhoton::InputAnimationState(enum_PlayerAnimationState _State)
{
	if (m_pListener)
	{
		m_pListener->SetPlayerAnim(int(_State));
	}
}


//방에 들어가기 전에 playfab에서 받은 플레이어 데이터 채우고 연결하기
void AActor_SolaseadoPhoton::InitPlayerData_Implementation()
{
	///********** 여기에 데이터 채워넣기

	//플레이앱 코스튬 데이터)
	// LocalPlayer -> playfab 에서 값 받아오고
	//InputCharacterInfo(key_값 , 플레이앱 코스튬 데이터);

	const TArray<FString> &PlayFabData = LocalPlayer->UploadPlayer();

	DataCount = 0;

	for (auto it : PlayFabData)
	{
		FString str = "c"; // Costume Data
		str += FString::FromInt(DataCount++);
		InputCharacterInfo(str, it);
		SendPlayerInfo();
	}

	//임시로 enum_PlayerAnimationState::Idle을 넣어뒀다.
	//플레이어 폰에 애니메이션 상태 변수가 정착되면 거기에 있는 상태값을 여기에 넣어준다.
	InputAnimationState(enum_PlayerAnimationState::Idle);


	//쌓인 데이터 보내고 룸 입장하기
	m_pListener->InitJoinOrCreateRoom();
}


//코스튬 파츠 데이터를 포톤 서버에 전송
//파츠 이름을 여기에 FString 인자로 넣어주면 됩니다.
void AActor_SolaseadoPhoton::SendCostumeParts(FString Parts)
{
	FString str = "costume";
	InputCharacterInfo(str, Parts);
	SendPlayerInfo();
}


