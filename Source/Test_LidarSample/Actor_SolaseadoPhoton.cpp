// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_SolaseadoPhoton.h"

#include "Engine/LevelStreaming.h"

#include <time.h>

#define Photon_AppID "6b83afef-1da9-4b42-9281-da551443a54b"
#define Photon_ServerAddress "ns.exitgames.com"

// Sets default values
AActor_SolaseadoPhoton::AActor_SolaseadoPhoton() : serverAddress(TEXT(Photon_ServerAddress)), AppID(TEXT(Photon_AppID))
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<APawn_Player> FindtargetCharacter(TEXT("Blueprint'/Game/Project/Pawn/Solaseado/BP_OtherPlayer.BP_OtherPlayer_C'"));
	//FindtargetCharacter(TEXT("/Game/Project/BP_targetPlayer")); 
	if (FindtargetCharacter.Succeeded())
		targetCharacter = FindtargetCharacter.Class;
}

AActor_SolaseadoPhoton::~AActor_SolaseadoPhoton()
{
	if (m_pListener)
		delete m_pListener;
	if (m_pListener)
		delete m_pClient;
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
		//LocalPlayer->
	}
}

void AActor_SolaseadoPhoton::movePlayer(FVector Loc)
{
	if (CurrentLocation != Loc)
	{
		CurrentLocation = Loc;
		m_pListener->SetMovePlayer(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z);
	}
}

void AActor_SolaseadoPhoton::movePlayer(int vx, int vy, int vz)
{
	FVector Checker = FVector(vx, vy, vz);
	if (CurrentLocation != Checker)
	{
		CurrentLocation = Checker;
		m_pListener->SetMovePlayer(vx, vy, vz);
	}
}

void AActor_SolaseadoPhoton::movePlayerRotation(float fZ)
{
	if (CurrentRotation != fZ)
	{
		CurrentRotation = fZ;
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
			updateLocalPlayerPosion();
		}
	}
}



void AActor_SolaseadoPhoton::ConnectLogin(const FString& username)
{
	srand(GETTIMEMS());
	m_pListener = new PhotonListner_Solaseado(this);
	m_pClient = new ExitGames::LoadBalancing::Client(*m_pListener, TCHAR_TO_UTF8(*AppID), TCHAR_TO_UTF8(*appVersion)); //  (nByte)0U, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT
	m_pListener->SetClient(m_pClient);
	m_pListener->Connect(TCHAR_TO_UTF8(*username), TCHAR_TO_UTF8(*serverAddress));
}

void AActor_SolaseadoPhoton::DummyConnectLogin(const FString& username, APawn_Player* dummy)
{
	srand(GETTIMEMS());

	LocalPlayer = dummy;
	m_pListener = new PhotonListner_Solaseado(this);
	m_pClient = new ExitGames::LoadBalancing::Client(*m_pListener, TCHAR_TO_UTF8(*AppID), TCHAR_TO_UTF8(*appVersion)); //  (nByte)0U, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT
	m_pListener->SetClient(m_pClient);
	m_pListener->Connect(TCHAR_TO_UTF8(*username), TCHAR_TO_UTF8(*serverAddress));

}



// 텍스트 메세지 출력 
void AActor_SolaseadoPhoton::SendTextMessage(const FString& Message, const FString& type)
{
	if (m_pListener)
		m_pListener->TextMessage(TCHAR_TO_UTF8(*Message), TCHAR_TO_UTF8(*type));
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

void AActor_SolaseadoPhoton::setRegion()
{
	m_pClient->selectRegion("kr");
}

void AActor_SolaseadoPhoton::ErrorCheckMessage(const FString& message, int error)
{

}

//필드 초기화
void AActor_SolaseadoPhoton::InitPlayers(void)
{
	for (auto it : PlayerList)
	{
		it->Destroy();
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
		const char* Temp = TCHAR_TO_UTF8(*str);

		if (Custom.contains(Temp))
		{
			JString Costume = ((ValueObject<JString>*)Custom.getValue(Temp))->getDataCopy();

			ArrayPlayerCostume.Add(FString(UTF8_TO_TCHAR(Costume.UTF8Representation().cstr())));

			//UE_LOG(LogTemp, Log, TEXT("//AddPlayers Costunme %s :: %s"),*str ,*FString(UTF8_TO_TCHAR(Costume.UTF8Representation().cstr())));
		}
	}


	if (local)
	{
		// UE_LOG(LogTemp, Log, TEXT("// Add Local Player :: %s"), *name);
		PlayerList.Add(LocalPlayer);
		LocalPlayer->AddClentPlayerCount();
		if (LocalPlayer)
		{
			LocalPlayer->isInRoom = true;
			LocalPlayer->PlayerNr = playerNr;
			LocalPlayer->bLocal = true;
			//LocalPlayer->AddMainWidget();
			LocalPlayer->PlayerName = name;
			//LocalPlayer->setPlayerNameTag(name);

			//포톤으로 받은 코스튬 데이터를 전달한다.
			LocalPlayer->SetCostumeArray(ArrayPlayerCostume);
		}
	}
	else
	{
		// Character Anim (true = walk , false = idle)
		//
		// 
		// 
		//  chAnim = JString();
		//FString Anim = "An";
		//const char* TempAnim = TCHAR_TO_UTF8(*Anim);
		//if (Custom.contains(TempAnim))
		//{
		//	chAnim = ((ValueObject<JString>*)Custom.getValue(TempAnim))->getDataCopy();
		//}

		FActorSpawnParameters actorSpawnParam;
		actorSpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


		APawn_Player* target = GetWorld()->SpawnActor<APawn_Player>(targetCharacter, FTransform(), actorSpawnParam);
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
	서버에서 받아온 캐릭터 정보 갱신 구간
	*****************************************/
void AActor_SolaseadoPhoton::updatePlayerProperties(int playerNr, const Hashtable& changes)
{


	// 캐릭터 애니메이션 변경 적용 
	if (changes.contains("An"))
	{
		nByte Anim = ((ValueObject<nByte>*)changes.getValue("An"))->getDataCopy();
		// UE_LOG(LogTemp, Log, TEXT("// contains( An ) :: %d "), Anim);
		for (auto it : PlayerList)
		{
			if (it->PlayerNr == playerNr)
			{
				it->bWalk = (bool)Anim;
				return;
			}
		}
	}


	if (changes.contains("TT"))
	{
		int Tdate = ((ValueObject<int>*)changes.getValue("TT"))->getDataCopy();
		// UE_LOG(LogTemp, Log, TEXT("// contains( An ) :: %d "), Anim);
		for (auto it : PlayerList)
		{
			if (it->PlayerNr == playerNr)
			{
				//UE_LOG(LogTemp, Log, TEXT("// Load updatePlayerProperties :: %d"), (int)Tdate);
				return;
			}
		}
	}


	//코스튬 구간
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{

			TArray<FString> CostumeList;
			for (int i = 0; i < DataCount; i++)
			{
				FString str = "c"; // Costume Data

				str += FString::FromInt(i);
				const char* Temp = TCHAR_TO_UTF8(*str);
				JString Costume = ""; // 변경 코스튬

				if (changes.contains(Temp))
				{
					Costume = ((ValueObject<JString>*)changes.getValue(Temp))->getDataCopy();

					FString cc = FString(UTF8_TO_TCHAR(Costume.UTF8Representation().cstr()));

					//UE_LOG(LogTemp, Log, TEXT("// Load updatePlayerProperties Costume:: %s"), *cc);
					CostumeList.Add(cc);
				}
			}
			//for (int i = 0; i < 5; i++)
			//{
			//	FString str = "c"; // Costume Data
			//
			//	str += FString::FromInt(i);
			//	const char* Temp = TCHAR_TO_UTF8(*str);
			//	int Costume = 0; // 변경 코스튬	
			//
			//	if (changes.contains(Temp))
			//	{
			//		Costume = ((ValueObject<int>*)changes.getValue(Temp))->getDataCopy();
			//
			//		FCostume info;
			//		info.Type = enum_CostumeType(i);
			//		info.PartNumber = Costume;
			//
			//		CostumeList.Add(info);
			//	}
			//}

			//SetCustomCostume(playerNr, CostumeList);
			break;
		}
	}
}


void AActor_SolaseadoPhoton::setPlayerAnimationData(uint8 anim)
{
	if (m_pListener)
		m_pListener->setPlayerAnimationData(anim);
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

void AActor_SolaseadoPhoton::JoinOrCreateComplete()
{
	//UE_LOG(LogTemp, Log, TEXT("// JoinOrCreateComplete :: "));
	// #include "Actor_RosActor.h"	
	// 헤더 연결 오류 처리 // Blueprint Spawn 변경 
	//ConnectRosActor();
	ConnectPhotonCHat();
}

void AActor_SolaseadoPhoton::LeaveRoomComplete(void)
{
	//UE_LOG(LogTemp, Log, TEXT("// LeaveRoomComplete :: "));
	if (LocalPlayer)
	{
		LocalPlayer->isInRoom = false;
	}
}

void AActor_SolaseadoPhoton::CurrentRoomInfo(const ExitGames::Common::JString& name, nByte Count, nByte MaxCount)
{
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
	// 캐릭터 애니메이션 변경 적용 
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
void AActor_SolaseadoPhoton::InputCharacterInfo(FString _key, FString _value)
{
	//UE_LOG(LogTemp, Log, TEXT("// InputCharacterInfo "));
	m_pListener->SetChracterInfo(_key, _value);
}

// 저장한 캐릭터 데이터 보내기
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
		// 로컬 플레이어 위치 정보 
		movePlayer(LocalPlayer->GetActorLocation());
		movePlayerRotation(LocalPlayer->GetActorRotation().Yaw);
	}

}

//받은 코스튬 데이터로 플레이어의 아바타를 세팅해주는 함수
void AActor_SolaseadoPhoton::SetCustomCostume_Implementation(int playerNr, const TArray<FCostume>& arrayCostume)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			//여기에 아바타 세팅관련 추가해주면 됩니다.

			for (auto tCostume : arrayCostume)
			{
				//it->ChangeCostumeParts(tCostume.Type,tCostume.PartNumber);
			}
		}
	}
}

void AActor_SolaseadoPhoton::testdata()
{
	FString str = "c"; // Costume Data
	str += FString::FromInt(1);

	InputCharacterInfo(str, "11111");

	SendPlayerInfo();
}


//방에 들어가기 전에 playfab에서 받은 플레이어 데이터 채우고 연결하기
void AActor_SolaseadoPhoton::InitPlayerData_Implementation()
{
	///********** 여기에 데이터 채워넣기

	//플레이앱 코스튬 데이터)
	// LocalPlayer -> playfab 에서 값 받아오고
	//InputCharacterInfo(key_값 , 플레이앱 코스튬 데이터);

	const TArray<FString> &PlayFabData = LocalPlayer->UploadPlayer();

	//UE_LOG(LogTemp, Log, TEXT("// InitPlayerData_Implementation :: %d "), PlayFabData.Num());
	
	DataCount = 0;

	for (auto it : PlayFabData)
	{
		FString str = "c"; // Costume Data
		str += FString::FromInt(DataCount++);

		InputCharacterInfo(str, it);
	}

	//쌓인 데이터 보내고 룸 입장하기
	m_pListener->InitJoinOrCreateRoom();
}
