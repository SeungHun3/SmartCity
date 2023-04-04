// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_Photon.h"
#include "Character_Player.h"
#include "Engine/LevelStreaming.h"
#include <time.h>

#define Photon_AppID "6b83afef-1da9-4b42-9281-da551443a54b"
#define Photon_ServerAddress "ns.exitgames.com"

// Sets default values
AActor_Photon::AActor_Photon()
	:serverAddress(TEXT(Photon_ServerAddress)), AppID(TEXT(Photon_AppID))
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

AActor_Photon::~AActor_Photon()
{
	if (m_pListener)
		delete m_pListener;
	if (m_pListener)
		delete m_pClient;
}

// Called when the game starts or when spawned
void AActor_Photon::BeginPlay()
{
	Super::BeginPlay();
	this->SetTickableWhenPaused(true);

	const UWorld* world = GetWorld();
	if (world)
	{
		LocalPlayer = Cast<ACharacter_Player>(world->GetFirstPlayerController()->GetPawn());
	}
}

// Called every frame
void AActor_Photon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	unsigned long t = GETTIMEMS();
	if ((t - lastUpdateTime) > 100)
	{
		lastUpdateTime = t;
		m_pListener->service();
		if (LocalPlayer && LocalPlayer->isInRoom)
		{
			// 로컬 플레이어 위치 정보 
			movePlayer(LocalPlayer->GetActorLocation());
			movePlayerRotation(LocalPlayer->GetActorRotation().Yaw);
		}
	}
}

void AActor_Photon::setRegion()
{
	// 지역 선택 "kr"
	m_pClient->selectRegion("kr");
}

// 에러 메세지 체크
void AActor_Photon::ErrorCheckMessage(const FString& message, int error)
{
	// UE_LOG(LogTemp, Warning, TEXT("// ErrorCheckMessage :: %s (%d)"), *message, error);
}

// Login
void AActor_Photon::ConnectLogin(const FString& username)
{
	srand(GETTIMEMS());
	//m_pListener = new SH_PhotonListener(this);
	//m_pClient = new ExitGames::LoadBalancing::Client(*m_pListener, TCHAR_TO_UTF8(*AppID), TCHAR_TO_UTF8(*appVersion)); //  (nByte)0U, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT
	//m_pListener->SetClient(m_pClient);
	//m_pListener->Connect(TCHAR_TO_UTF8(*username), TCHAR_TO_UTF8(*serverAddress));
}

// 텍스트 메세지 출력 
void AActor_Photon::SendTextMessage(const FString& Message, const FString& type)
{
	if (m_pListener)
		m_pListener->TextMessage(TCHAR_TO_UTF8(*Message), TCHAR_TO_UTF8(*type));
}

// 재접속 
void AActor_Photon::ReconnectMessage()
{
	// UE_LOG(LogTemp, Log, TEXT("// reconnect Messsage Event :: Destroyed AActor_PhotonClient "));
	// DisconnectComplete.Broadcast();
	this->Destroy();
}

void AActor_Photon::PhotonDisconnect()
{
	if (m_pClient)
		m_pClient->disconnect();
}
// 캐릭터 이동 & 회전
void AActor_Photon::movePlayer(FVector Loc)
{
	if (CurrentLocation != Loc)
	{
		CurrentLocation = Loc;
		m_pListener->SetMovePlayer(CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z);
	}
}

void AActor_Photon::movePlayer(int vx, int vy, int vz)
{
	FVector Checker = FVector(vx, vy, vz);
	if (CurrentLocation != Checker)
	{
		CurrentLocation = Checker;
		m_pListener->SetMovePlayer(vx, vy, vz);
	}
	
}
// Rotation
void AActor_Photon::movePlayerRotation(float fZ)
{
	if (CurrentRotation != fZ)
	{
		CurrentRotation = fZ;
		m_pListener->SetMovePlayerRotation(fZ);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Photon Custom Event 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AActor_Photon::AddPlayers(int playerNr, const ExitGames::Common::JString& playerName, bool local, const ExitGames::Common::Hashtable& Custom)
{
	FString name = FString(UTF8_TO_TCHAR(playerName.UTF8Representation().cstr()));

	//// 추가 커스텀 데이터 >> 애니메이션 추가
	//int size = Custom.getSize();

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
			LocalPlayer->AddMainWidget();
			LocalPlayer->PlayerName = name;
			LocalPlayer->setPlayerNameTag(name);
		}
	}
	else
	{
		// Character Anim (true = walk , false = idle)
		//JString chAnim = JString();
		//FString Anim = "An";
		//const char* TempAnim = TCHAR_TO_UTF8(*Anim);
		//if (Custom.contains(TempAnim))
		//{
		//	chAnim = ((ValueObject<JString>*)Custom.getValue(TempAnim))->getDataCopy();
		//}

		FActorSpawnParameters actorSpawnParam;
		actorSpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//for (auto it : GetWorld()->GetStreamingLevels())
		//{
		//	// UE_LOG(LogTemp, Log, TEXT("// GetStreamingLevels :: %s"), *it->GetName());
		//	actorSpawnParam.OverrideLevel = it->GetLoadedLevel();
		//}

		// targetCharacter 외형 설정 캐릭터 추가
		ACharacter_Player* target = GetWorld()->SpawnActor<ACharacter_Player>(targetCharacter, FTransform(), actorSpawnParam);
		if (target)
		{
			// 타겟 플레이어 이름
			target->PlayerName = name;
			target->PlayerNr = playerNr;
			target->setPlayerNameTag(name);

			// 애니메이션 데이터 
			// FString str = FString(UTF8_TO_TCHAR(chAnim.UTF8Representation().cstr()));
			// target->bWalk = (bool)FCString::Atoi(*str);

			PlayerList.Add(target);
			LocalPlayer->AddClentPlayerCount();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////
// photon : 접속자 커스텀 데이터 변경
void AActor_Photon::updatePlayerProperties(int playerNr, const Hashtable& changes)
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
}

// Player 애니메이션 변경시 호출 
void AActor_Photon::setPlayerAnimationData(uint8 anim)
{
	if (m_pListener)
		m_pListener->setPlayerAnimationData(anim);
}

void AActor_Photon::RemovePlayer(int playerNr)
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

// 캐릭터 움직임
void AActor_Photon::GetMovePlayer(int playerNr, int vX, int vY, int vZ)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			// 높이값 고정 으로 변경 vZ
			it->SetActorLocation(FVector(vX, vY, 66.f));
			return;
		}
	}
}
void AActor_Photon::GetMovePlayerRotation(int playerNr, float fX)
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

// 필드 초기화
void AActor_Photon::InitPlayers(void)
{
	// 플레이어 캐릭터 제거
	for (auto it : PlayerList)
	{
		it->Destroy();
	}
	PlayerList.Reset();
}

void AActor_Photon::ConnectComplete(void)
{
	// #include "Actor_RosActor.h"	
	// 헤더 연결 오류 처리 // Blueprint Spawn 변경 
	ConnectRosActor();
}

void AActor_Photon::CreateChannelComplete(const ExitGames::Common::JString& map, const ExitGames::Common::JString& channel)
{

}

void AActor_Photon::CreateRoomComplete(const ExitGames::Common::JString& map)
{

}

void AActor_Photon::JoinRoomComplete(const ExitGames::Common::JString& map, const ExitGames::Common::JString& channel)
{

}

void AActor_Photon::JoinOrCreateComplete()
{
	// UE_LOG(LogTemp, Log, TEXT("// JoinOrCreateComplete :: "));

}

void AActor_Photon::LeaveRoomComplete(void)
{
	// UE_LOG(LogTemp, Log, TEXT("// LeaveRoomComplete :: "));
	if (LocalPlayer)
	{
		LocalPlayer->isInRoom = false;
	}
}

void AActor_Photon::CurrentRoomInfo(const ExitGames::Common::JString& name, nByte Count, nByte MaxCount)
{

}

void AActor_Photon::getTextMessage(int playerNr, const ExitGames::Common::JString& Message, const ExitGames::Common::JString& Type)
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
			Enum_TextType type = (Enum_TextType)FCString::Atoi(*TextType);
			switch (type)
			{
				case Enum_TextType::Normal:
				{
					// 말풍선 띄우기
					it->OpenMessageBox(TextMessage);
					LocalPlayer->AddChatList(it->PlayerName, TextMessage);
					break;
				}
				case Enum_TextType::World:
				{
					// 화면 전체 메세지 띄우기
					LocalPlayer->OpenWorldMessageBox(TextMessage, true);
					break;
				}
				case Enum_TextType::infinite:
				{
					LocalPlayer->OpenWorldMessageBox(TextMessage, false);
					break;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// photon : 룸 커스텀 데이터 변경 호출
void AActor_Photon::ChangeRoomEventProperty(uint8 Ev)
{
	if (m_pListener)
		m_pListener->setRoomEventProperties(Ev);
}
// 룸 커스텀 데이터 변경 콜벡
void AActor_Photon::updateRoomProperties(const Hashtable& changes)
{
	// 캐릭터 애니메이션 변경 적용 
	if (changes.contains("Ev"))
	{
		nByte roomEvent = ((ValueObject<nByte>*)changes.getValue("Ev"))->getDataCopy();
		LocalPlayer->ChangeRoomEvent((uint8)roomEvent);
	}
}
// 이벤트 일시정지 호출
void AActor_Photon::ChangeEventPause(bool ev)
{
	if (m_pListener)
		m_pListener->setEventPause(ev);
}
// 이벤트 일시정지
void AActor_Photon::getEventPause(bool ev)
{
	if(LocalPlayer)
		LocalPlayer->ChangeEventPause(ev);
}
