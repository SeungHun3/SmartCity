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
	m_pClient = new ExitGames::LoadBalancing::Client(*m_pListener, TCHAR_TO_UTF8(*AppID), TCHAR_TO_UTF8(*appVersion),
		Photon::ConnectionProtocol::DEFAULT, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT, false); //  (nByte)0U, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT
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



// �ؽ�Ʈ �޼��� ��� 
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
	//UE_LOG(LogTemp, Log, TEXT("//setRegion"));
	m_pClient->selectRegion("kr");
}

void AActor_SolaseadoPhoton::ErrorCheckMessage(const FString& message, int error)
{

}

//�ʵ� �ʱ�ȭ
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

	//// �߰� Ŀ���� ������ >> �ִϸ��̼� �߰�
	//int size = Custom.getSize();

	//�Է¹��� �ڽ�Ƭ ����
	TArray<FString> ArrayPlayerCostume;
	for (int i = 0; i < DataCount; ++i)
	{
		FString str = "c"; // Costume Data
		str += FString::FromInt(i);

		//�����ڵ�
		//char* Temp = TCHAR_TO_UTF8(*str);

		//������ �ڵ�
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
			LocalPlayer->PlayerNr = playerNr;
			LocalPlayer->bLocal = true;
			//LocalPlayer->AddMainWidget();
			LocalPlayer->PlayerName = name;
			//LocalPlayer->setPlayerNameTag(name);

			//�������� ���� �ڽ�Ƭ �����͸� �����Ѵ�.
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
			// Ÿ�� �÷��̾� �̸�
			target->PlayerName = name;
			target->PlayerNr = playerNr;
			//target->setPlayerNameTag(name);

			// �ִϸ��̼� ������ 
			// FString str = FString(UTF8_TO_TCHAR(chAnim.UTF8Representation().cstr()));
			// target->bWalk = (bool)FCString::Atoi(*str);

			PlayerList.Add(target);
			LocalPlayer->AddClentPlayerCount();

			//�������� ���� �ڽ�Ƭ �����͸� �����Ѵ�.
			target->SetCostumeArray(ArrayPlayerCostume);
		}
	}
}

//�������� �÷��̾� ����
void AActor_SolaseadoPhoton::RemovePlayer(int playerNr)
{
	// ������ ����Ʈ ���� 
	// RemovePlayerEvent.Broadcast(playerNr);
	for (auto it : PlayerList)
	{
		// ������ ����
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
	�������� �޾ƿ� �ٸ� �÷��̾���� ���� ���� ����
	*****************************************/
void AActor_SolaseadoPhoton::updatePlayerProperties(int playerNr, const Hashtable& changes)
{

	// ĳ���� �ִϸ��̼� ���� ���� 
	if (changes.contains("An"))
	{
		uint8 Anim = ((ValueObject<uint8>*)changes.getValue("An"))->getDataCopy();
		for (auto it : PlayerList)
		{
			if (it->PlayerNr == playerNr)
			{
				//�Ʒ� ����ó�� Pawn_Player�� �ִ� ������ ��������ָ� �˴ϴ�.
				//Pawn Player�� �ش� ������ ��� ������� �ʾ� ������ �߻����״� �ּ�ó�� �Ͻðų� �ش� ������ ������ֽð� Ŀ���ϸ� �������״� 
				//���� ��Ź�帳�ϴ�.
				//����
				it->eAnimationState = (enum_PlayerAnimationState)Anim;
			}
		}
	}

	//�ڽ�Ƭ ����(n=DataCount) ::  "C_0" ~ "C_n" 
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{

			TArray<FString> CostumeList;
			for (int i = 0; i < DataCount; i++)
			{
				FString str = "c"; // Costume Data
				str += FString::FromInt(i);

				const TCHAR* TempTCHAR = *str;
				const char* Temp = new char[FCString::Strlen(TempTCHAR) + 1];
				FCStringAnsi::Strcpy(const_cast<char*>(Temp), FCString::Strlen(TempTCHAR) + 1, TCHAR_TO_UTF8(TempTCHAR));

				JString Costume = ""; // ���� �ڽ�Ƭ

				if (changes.contains(Temp))
				{
					Costume = ((ValueObject<JString>*)changes.getValue(Temp))->getDataCopy();
					FString cc = FString(UTF8_TO_TCHAR(Costume.UTF8Representation().cstr()));
					CostumeList.Add(cc);
				}
			}
			it->SetCostumeArray(CostumeList);
			break;
		}
	}
}


void AActor_SolaseadoPhoton::setPlayerAnimationData(uint8 anim)
{
	if (m_pListener)
		m_pListener->setPlayerAnimationData(anim);
}



//ĳ���� ������
void AActor_SolaseadoPhoton::GetMovePlayer(int playerNr, int vX, int vY, int vZ)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			// ���̰� ���� ���� ���� vZ
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
	// ��� ���� ���� ó�� // Blueprint Spawn ���� 
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
				// ��ǳ�� ����
				//it->OpenMessageBox(TextMessage);
				//LocalPlayer->AddChatList(it->PlayerName, TextMessage);
				break;
			}
			case Enum_TextType3::World:
			{
				// ȭ�� ��ü �޼��� ����
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


// photon : �� Ŀ���� ������ ���� ȣ��
void AActor_SolaseadoPhoton::ChangeRoomEventProperty(uint8 Ev)
{
	if (m_pListener)
		m_pListener->setRoomEventProperties(Ev);
}

// �� Ŀ���� ������ ���� �ݺ�
void AActor_SolaseadoPhoton::updateRoomProperties(const Hashtable& changes)
{
	// 
	if (changes.contains("Ev"))
	{
		nByte roomEvent = ((ValueObject<nByte>*)changes.getValue("Ev"))->getDataCopy();
		//LocalPlayer->ChangeRoomEvent((uint8)roomEvent);
	}
}

// �̺�Ʈ �Ͻ����� ȣ��
void AActor_SolaseadoPhoton::ChangeEventPause(bool ev)
{
	//if (m_pListener)
	//	m_pListener->setEventPause(ev);
}

// �̺�Ʈ �Ͻ�����
void AActor_SolaseadoPhoton::getEventPause(bool ev)
{
	//if (LocalPlayer)
	//	LocalPlayer->ChangeEventPause(ev);
}


// ĳ���� ������ ����
// 
void AActor_SolaseadoPhoton::InputCharacterInfo(FString _key, FString _value)
{
	//UE_LOG(LogTemp, Log, TEXT("// InputCharacterInfo "));
	m_pListener->SetChracterInfo(_key, _value);
}

// ������ ĳ���� ������ ������
// ���� ���� �߰�
void AActor_SolaseadoPhoton::SendPlayerInfo()
{
	//UE_LOG(LogTemp, Log, TEXT("// SendPlayerInfo "));
	m_pListener->SendCharacterInfo();
}


//���� �÷��̾� ��ġ ����
void AActor_SolaseadoPhoton::updateLocalPlayerPosion()
{
	if (LocalPlayer && LocalPlayer->isInRoom)
	{
		// ���� �÷��̾� ��ġ ���� 
		movePlayer(LocalPlayer->GetActorLocation());
		movePlayerRotation(LocalPlayer->GetActorRotation().Yaw);
	}

}

//���� �ڽ�Ƭ �����ͷ� �÷��̾��� �ƹ�Ÿ�� �������ִ� �Լ�
//��������� �ʽ��ϴ�.
void AActor_SolaseadoPhoton::SetCustomCostume_Implementation(int playerNr, const TArray<FCostume>& arrayCostume)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			//���⿡ �ƹ�Ÿ ���ð��� �߰����ָ� �˴ϴ�.

			for (auto tCostume : arrayCostume)
			{
				//it->ChangeCostumeParts(tCostume.Type,tCostume.PartNumber);
			}
		}
	}
}

//���� �ִϸ��̼��� ������ �����ϱ�
/*
Play_Pawn���� �������� �ִϸ��̼� ���°��� �ٲ������ ���⿡ �־��ָ� �ٸ� �÷��̾�� ���°��� ���޵˴ϴ�.
*/
void AActor_SolaseadoPhoton::InputAnimationState(enum_PlayerAnimationState _State)
{
	m_pListener->SendPlayerAnimState((uint8)_State);
}


//�濡 ���� ���� playfab���� ���� �÷��̾� ������ ä��� �����ϱ�
void AActor_SolaseadoPhoton::InitPlayerData_Implementation()
{
	///********** ���⿡ ������ ä���ֱ�

	//�÷��̾� �ڽ�Ƭ ������)
	// LocalPlayer -> playfab ���� �� �޾ƿ���
	//InputCharacterInfo(key_�� , �÷��̾� �ڽ�Ƭ ������);

	const TArray<FString> &PlayFabData = LocalPlayer->UploadPlayer();

	//UE_LOG(LogTemp, Log, TEXT("// InitPlayerData_Implementation :: %d "), PlayFabData.Num());
	
	DataCount = 0;

	for (auto it : PlayFabData)
	{
		FString str = "c"; // Costume Data
		str += FString::FromInt(DataCount++);
		InputCharacterInfo(str, it);
	}

	//�ӽ÷� enum_PlayerAnimationState::Idle�� �־�״�.
	//�÷��̾� ���� �ִϸ��̼� ���� ������ �����Ǹ� �ű⿡ �ִ� ���°��� ���⿡ �־��ش�.
	InputAnimationState(enum_PlayerAnimationState::Idle);


	//���� ������ ������ �� �����ϱ�
	m_pListener->InitJoinOrCreateRoom();
}
