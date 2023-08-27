// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_SolaseadoPhoton.h"
#include "GameModeBase_Solaseado.h"
#include "Actor_PhotonChat.h"
#include "Actor_PhotonVoice.h"
#include "Pawn_NPC.h"

#include "Engine/LevelStreaming.h"

#include <time.h>

#define Photon_AppID "6b83afef-1da9-4b42-9281-da551443a54b"
#define Photon_ServerAddress "ns.exitgames.com"

// Sets default values
AActor_SolaseadoPhoton::AActor_SolaseadoPhoton() : serverAddress(TEXT(Photon_ServerAddress)), AppID(TEXT(Photon_AppID))
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> FindNPCTableData(TEXT("DataTable'/Game/Project/DataTable/NPC/Data_NPC.Data_NPC'"));
	if (FindNPCTableData.Succeeded())
		CustomNPCDataTable = FindNPCTableData.Object;
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

}



void AActor_SolaseadoPhoton::ConnectLogin(const FString& username)
{
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
	AGameModeBase_Solaseado* GM_Solaseado = Cast<AGameModeBase_Solaseado>(GetWorld()->GetAuthGameMode());
	if (!IsChangingRoom)
	{
		GM_Solaseado->PhotonVoice->setIschanging(true); // 보이스 바꾸기
		IsChangingRoom = true; //  == > JoinOrCreateComplete  될때 false로 바꿈 // 중복클릭 방지
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

	TArray<FString> PlayerNames;

	ExitGames::Common::JVector<ExitGames::LoadBalancing::Player*> Players = m_pClient->getCurrentlyJoinedRoom().getPlayers();
	for (int i = 0; i < (int)Players.getSize(); i++)
	{
		FString PlayerName = Players[i]->getName().UTF8Representation().cstr();
		PlayerNames.Add(PlayerName);
	}

	ChangeViewCount(Channel_Count,PlayerNames);
}

void AActor_SolaseadoPhoton::CloseDummy()
{
	dummy_pClient->disconnect();
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
	Blueprint_ResetPlayer();
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
		if (LocalPlayer)
		{
			LocalPlayer->isInRoom = true;
			LocalPlayer->isInLobby = false;
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

			PlayerList.Add(target);
			Blueprint_AddPlayer(playerNr , name);

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
			it->Destroy();
			PlayerList.Remove(it);
			// Widget Event 
			Blueprint_RemovePlayer(playerNr);
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
	AGameModeBase_Solaseado* GM_Solaseado = Cast<AGameModeBase_Solaseado>(GetWorld()->GetAuthGameMode()); // moveLevel_ SimPoly1, Fade
	if (GM_Solaseado)
	{
		GM_Solaseado->PhotonChat->Chat_ResetJoinChannel(RoomFullName);
		GM_Solaseado->PhotonVoice->Voice_ChangeOrJoinRoom(RoomFullName);

		GM_Solaseado->MoveLevel(enum_Level::SimPoly1, true);
		IsChangingRoom = false;
	}
}
// 룸 나갈때 호출
void AActor_SolaseadoPhoton::LeaveRoomComplete(void)
{
	if (LocalPlayer)
	{
		LocalPlayer->isInRoom = false;
		LocalPlayer->isInLobby = true;
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

	FString str = "NPCSize"; // + FString::FromInt(0);
	Object const* objn = changes.getValue(TCHAR_TO_UTF8(*str));
	if (objn)
	{
		int n = (((ValueObject<int*>*)objn)->getDataCopy())[0];

		for (int i = 1; i <= n; ++i)
		{
			str = "NPC"+FString::FromInt(i);
			Object const* objTemp = changes.getValue(TCHAR_TO_UTF8(*str));

			if (objTemp)
			{
				float* NpcLocationArray = ((ValueObject<float*>*)objTemp)->getDataCopy();
				AddNpc(str, FVector(NpcLocationArray[0], NpcLocationArray[1], NpcLocationArray[2]), FVector(1.0f), FVector(1.0f));
			}
		}
	}

	if (NPCList.IsValidIndex(0))
	{
		for (auto it : NPCList)
		{		
			Object const* objTemp = changes.getValue(TCHAR_TO_UTF8(*(it->CharacterID)));
			if (objTemp)
			{
				float* NpcLocationArray = ((ValueObject<float*>*)objTemp)->getDataCopy();
				it->NPCMove(FVector(NpcLocationArray[0], NpcLocationArray[1], NpcLocationArray[2]));
			}
		}
	}
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


int AActor_SolaseadoPhoton::GetMyChannelNumber()
{
	return m_pListener->getRoomCount();
}

void AActor_SolaseadoPhoton::PhotonMove(FVector Loc)
{
	m_pListener->Move(Loc);
}

void AActor_SolaseadoPhoton::PhotonStop()
{
	m_pListener->MoveStop();
}

void AActor_SolaseadoPhoton::PhotonFinish(FVector Loc)
{
	m_pListener->MoveStopFinish(Loc);
}

void AActor_SolaseadoPhoton::UpdateMove(int PlayerNum, FVector Loc)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == PlayerNum)
		{
			it->Pawn_Move(Loc);
			return;
		}
	}
}
void AActor_SolaseadoPhoton::UpdateStop(int PlayerNum)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == PlayerNum)
		{
			it->Pawn_StopMove();
			return;
		}
	}
}

void AActor_SolaseadoPhoton::UpdateStopFinished(int PlayerNum, FVector Loc)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == PlayerNum)
		{
			it->Pawn_StopMoveFinish(Loc);
			return;
		}
	}
}


///NPC ///////////////////////////////////////

//처음 시작할때 NPC 세팅
void AActor_SolaseadoPhoton::InitNpc()
{
	int count = 0;
	TArray<FString> aID;
	TArray<FVector> aLoc;

	for (auto it : CustomNPCDataTable->GetRowNames())
	{
		FNpcCustomizing_Struct* NPCTable = CustomNPCDataTable->FindRow<FNpcCustomizing_Struct>(it, TEXT(""));

		FString NpcId = it.ToString();

		aID.Add(NpcId);
		aLoc.Add(NPCTable->NpcLocation);

		AddNpc(NpcId, NPCTable->NpcLocation, FVector(1.0f), FVector(1.0f));
	}

	m_pListener->UpdateNPC(aID, aLoc);
}


//멀티 NPC 추가
void AActor_SolaseadoPhoton::AddNpc(const FString& NpcId, FVector Loc, FVector Rot, FVector Scale)
{
	FActorSpawnParameters actorSpawnParam;
	actorSpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APawn_NPC* target = GetWorld()->SpawnActor<APawn_NPC>(targetNPC, FTransform(FRotator(Rot.X, Rot.Y, Rot.Z), Loc, Scale), actorSpawnParam);
	if (target)
	{
		NPCList.Add(target);
		target->eNPCType= enum_NPCMultiType::Multi;
		target->CharacterID = NpcId;
		target->InitNPC(NpcId);
	}
}


// 채널 이동 후 남은 NPC 제거
void AActor_SolaseadoPhoton::ClearNpc()
{
	for (auto it : NPCList)
	{
		it->SelfDestroy();
	}
	NPCList.Empty();
}

//NPC 이동 좌표를 서버에 보낸다.
void AActor_SolaseadoPhoton::NPCMove(const APawn_NPC* pawnNPC,FVector NpcLocation)
{
	m_pListener->SetNPCMove(pawnNPC->CharacterID, NpcLocation);
}

///NPC ///////////////////////////////////////

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


//// 친구

void AActor_SolaseadoPhoton::SendFriendInfo(int Target,FString PlayfabID)
{
	if (m_pListener)
	{
		m_pListener->SetFriendRequest(Target,PlayfabID);
	}
}

//다른 플레이어에게 받은 친구 신청목록 함수에 처리한다.
void AActor_SolaseadoPhoton::RecvFriendRequest(int PlayerNum,const FString& FriendPlayFabID)
{
	FString PlayerName;

	for (auto it : PlayerList)
	{
		if (it->PlayerNr == PlayerNum)
		{
			PlayerName = it->PlayerName;
			LocalPlayer->UpdateFriendRequests(FriendPlayFabID, it->PlayerName);
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("// RecvFriendRequest :: %s"), *FriendPlayFabID);
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


