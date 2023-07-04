// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_SolaseadoPhoton.h"
#include "GameModeBase_Solaseado.h"
#include "Actor_PhotonChat.h"

#include "Engine/LevelStreaming.h"

#include <time.h>

#define Photon_AppID "6b83afef-1da9-4b42-9281-da551443a54b"
#define Photon_ServerAddress "ns.exitgames.com"

//1000ms=1second ������ �� ���� ������Ʈ(�ӽ� 1��)
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

	//�÷��̾� ���ǵ� �ӽ� ó��
	moveSpeed = 500.0f;
	//�� �Ÿ� �̻� �־����� ���� ��ġ ������ ���ش�.
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

	//���� �׽�Ʈ ���� �̵� ����ȭ �κ��� bOnTimeMove�� On/Off�Ѵ�
	//�� ������ �ð��� Update_Move_Time(1��)�� ���� �� ���� ��ġ ������Ʈ
	if (bOnTimeMove && bIsMoving && lastMoveTime+t-startMoveTime> Update_Move_Time)
	{
		lastMoveTime = 0;
		startMoveTime = t;
		
		//ȸ��
		movePlayerRotation(LocalPlayer->GetActorRotation().Yaw);
		//��ġ
		MovePlayerAndTime(LocalPlayer->GetActorLocation().X, LocalPlayer->GetActorLocation().Y, m_pClient->getServerTime() % 10000);
	}
}



void AActor_SolaseadoPhoton::ConnectLogin(const FString& username)
{
	//���� ä�� connect
	ConnectPhotonCHat(); // �������Ʈ ������ photonchat������� // JoinOrCreateComplete �� ������

	srand(GETTIMEMS());
	m_pListener = new PhotonListner_Solaseado(this);
	m_pClient = new ExitGames::LoadBalancing::Client(*m_pListener, TCHAR_TO_UTF8(*AppID), TCHAR_TO_UTF8(*appVersion),
		Photon::ConnectionProtocol::DEFAULT, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT, false); //  (nByte)0U, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT
	m_pListener->SetClient(m_pClient);
	m_pListener->Connect(TCHAR_TO_UTF8(*username), TCHAR_TO_UTF8(*serverAddress));
	
	
	//���̻�����
	dummy_pListener = new PhotonListner_Solaseado(this);
	dummy_pClient = new ExitGames::LoadBalancing::Client(*dummy_pListener, TCHAR_TO_UTF8(*AppID), TCHAR_TO_UTF8(*appVersion),
		Photon::ConnectionProtocol::DEFAULT, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT, false); //  (nByte)0U, false, ExitGames::LoadBalancing::RegionSelectionMode::SELECT
	//����
	dummy_pListener->setDummy(true);
	dummy_pListener->SetClient(dummy_pClient);
}

void AActor_SolaseadoPhoton::ChangeRoom(int Number)
{
	if (!IsChangingRoom)
	{
		IsChangingRoom = true; //  == > JoinOrCreateComplete  ���� ������ �� false�� �ٲ� // �ߺ�Ŭ�� ����
		m_pListener->ChangeRoomNumber(Number);
		m_pClient->opLeaveRoom(); // ==> LeaveRoomComplete
	}
}

///////////////////////////////////////////////////////////////////////
///////////////////Dummy or RoomSetting ����
void AActor_SolaseadoPhoton::OpenDummy()
{
	FString FuserName = m_pClient->getLocalPlayer().getName().UTF8Representation().cstr();
	FString DummyName = "dummy_" + FuserName;
	//����
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
////////////////////////////////////////////////Dummy or Room Setting��


// �ؽ�Ʈ �޼��� ��� 
void AActor_SolaseadoPhoton::SendTextMessage(const FString& Message, const FString& type)
{
	if (m_pListener)
		m_pListener->TextMessage(TCHAR_TO_UTF8(*Message), TCHAR_TO_UTF8(*type));
}


//Ŀ�ǵ带 �Է������� �� �Լ��� �Ἥ ���� ������ �ѷ��ش�.
//�̵��� �������� �޾����� ó���Ѵ�.
//������ �Է��� �ƴ����� ���� �˻�� �ٸ� �Լ��� ó���ϰ� �Է¹޴´�.
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

//���� ������� ����
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

//�ʵ� �ʱ�ȭ
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
			LocalPlayer->isInLoby = false;
			LocalPlayer->PlayerNr = playerNr;
			LocalPlayer->bLocal = true;
			LocalPlayer->PlayerName = name;
			//LocalPlayer->AddMainWidget();
			//LocalPlayer->setPlayerNameTag(name);

			//�������� ���� �ڽ�Ƭ �����͸� �����Ѵ�.
			LocalPlayer->SetCostumeArray(ArrayPlayerCostume);

		}
	}
	else
	{
		FActorSpawnParameters actorSpawnParam;
		actorSpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APawn_Player* target = GetWorld()->SpawnActor<APawn_Player>(targetCharacter, FTransform(), actorSpawnParam);
		//target->SpawnDefaultController();//�̰� ��� �����Ʈ�� �ȸ�����, �������Ʈ���� auto possess ai�� ���� ����
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

			//UE_LOG(LogTemp, Log, TEXT("// addplayer yaw :: %d , local : %d"), playerNr, LocalPlayer->PlayerNr);
			MoveAndRotation(LocalPlayer->GetActorLocation(), LocalPlayer->GetActorRotation().Yaw);

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
			//�ڽ�Ƭ ����(n=DataCount) ::  "C_0" ~ "C_n" 
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

					FMemory::Free((void*)Temp);
				}
				it->SetCostumeArray(CostumeList);
			}

			//���� �ڽ�Ƭ ���� ������Ʈ ����
			{
				TArray<FString> CostumeList;

				FString str = "costume"; // Costume Data

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


//�������� �÷������� �Է� �����͸� �Է¹޴´�.
//ó���� ������ ���� �ϴ� Tick���� ó���Ѵ�.
void AActor_SolaseadoPhoton::GetMovePlayerCommand(int playerNr, int iCommand)
{
	for (auto it : PlayerList)
	{
		if (it->PlayerNr == playerNr)
		{
			it->PhotonTimer = GETTIMEMS() % 10000;
			//������ �׽�Ʈ��(�׽�Ʈ ������ �ʿ����)
			//m_pListener->SendTestDelay(it->PhotonTimer);

			//���� �׽�Ʈ ���� �̵� ����ȭ �κ��� bOnTimeMove�� On/Off�Ѵ�
			if (it->InputMoveCommand((enum_InputPlayer)iCommand) && playerNr == LocalPlayer->PlayerNr && bOnTimeMove)
			{
				//������ ������Ʈ üũ
				//
				if ((it->fForward || it->fRight) && !bIsMoving)
				{
					// ������ ������Ʈ ����
					// ���� �� �������� ��� ������ �ð��� �����ϱ� ���� ���� �ð��� �����Ѵ�.
					bIsMoving = true;
					startMoveTime = GETTIMEMS();
					InputAnimationState(enum_PlayerAnimationState::Walk);
					

					//UE_LOG(LogTemp, Log, TEXT("//Start Walk x== %f, y== %f"), LocalPlayer->fForward, LocalPlayer->fRight);
					//UE_LOG(LogTemp, Log, TEXT("//Check Moving Time"));
				}
				else if ((it->fForward == 0 && it->fRight == 0) && bIsMoving)
				{
					// ������ ������Ʈ ����
					// ������ �ð��� ������ ó���ϱ� ���� lastMoveTime�� �����Ѵ�.
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

//���� ��������� ����
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

			//���� ��ġ + �պ��ð����� ������ �̵� ��ġ
			FVector lerpVector = FVector(vX, vY, PlayerHeight)+ (it->GetActorForwardVector() * it->fForward + it->GetActorRightVector() * it->fRight).GetSafeNormal() * tTime;
			
			//UE_LOG(LogTemp, Log, TEXT("//lerp x== %f, y== %f, tTime==%f"), lerpVector.X, lerpVector.Y, tTime);
			//UE_LOG(LogTemp, Log, TEXT("//fForward== %f, fRight== %f"), it->fForward, it->fRight);
			
			//�÷��̾� ���� �Ÿ��� 2m(lerpDistance) �̻� ���̰� ���� ��ġ���� �������ش�.
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

// �뿡 �������� ȣ��
void AActor_SolaseadoPhoton::JoinOrCreateComplete(const FString& RoomFullName)
{
	//UE_LOG(LogTemp, Log, TEXT("// JoinOrCreateComplete :: "));
	// #include "Actor_RosActor.h"	
	// ��� ���� ���� ó�� // Blueprint Spawn ���� 
	//ConnectRosActor();



	if (PhotonChat) // �濡 ������ ���������� ä�ù� ����
	{
		// ���ӽ�Ű���� ����Ŭ���尡 ������ �Ϸ�Ǹ� ���ȣ�� ��� ä�ü����� subscripe���� �Ѱ��ֱ�
		PhotonChat->Chat_ResetJoinChannel(RoomFullName);
	}

	AGameModeBase_Solaseado* GM_Solaseado = Cast<AGameModeBase_Solaseado>(GetWorld()->GetAuthGameMode()); // moveLevel_ SimPoly1, Fade
	GM_Solaseado->MoveLevel(enum_Level::SimPoly1, true);
	IsChangingRoom = false;
}
// �� ������ ȣ��
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
		//UE_LOG(LogTemp, Log, TEXT("// updateLocalPlayerPosion yaw: %f , x=%f ,y=%f ,z=%f "), LocalPlayer->GetActorRotation().Yaw, LocalPlayer->GetActorLocation().X, LocalPlayer->GetActorLocation().Y, LocalPlayer->GetActorLocation().Z);
		MoveAndRotation(LocalPlayer->GetActorLocation(), LocalPlayer->GetActorRotation().Yaw);
	}
}


//���� �ִϸ��̼��� ������ �����ϱ�
/*
Play_Pawn���� �������� �ִϸ��̼� ���°��� �ٲ������ ���⿡ �־��ָ� �ٸ� �÷��̾�� ���°��� ���޵˴ϴ�.
*/
void AActor_SolaseadoPhoton::InputAnimationState(enum_PlayerAnimationState _State)
{
	if (m_pListener)
	{
		m_pListener->SetPlayerAnim(int(_State));
	}
}


//�濡 ���� ���� playfab���� ���� �÷��̾� ������ ä��� �����ϱ�
void AActor_SolaseadoPhoton::InitPlayerData_Implementation()
{
	///********** ���⿡ ������ ä���ֱ�

	//�÷��̾� �ڽ�Ƭ ������)
	// LocalPlayer -> playfab ���� �� �޾ƿ���
	//InputCharacterInfo(key_�� , �÷��̾� �ڽ�Ƭ ������);

	const TArray<FString> &PlayFabData = LocalPlayer->UploadPlayer();

	DataCount = 0;

	for (auto it : PlayFabData)
	{
		FString str = "c"; // Costume Data
		str += FString::FromInt(DataCount++);
		InputCharacterInfo(str, it);
		SendPlayerInfo();
	}

	//�ӽ÷� enum_PlayerAnimationState::Idle�� �־�״�.
	//�÷��̾� ���� �ִϸ��̼� ���� ������ �����Ǹ� �ű⿡ �ִ� ���°��� ���⿡ �־��ش�.
	InputAnimationState(enum_PlayerAnimationState::Idle);


	//���� ������ ������ �� �����ϱ�
	m_pListener->InitJoinOrCreateRoom();
}


//�ڽ�Ƭ ���� �����͸� ���� ������ ����
//���� �̸��� ���⿡ FString ���ڷ� �־��ָ� �˴ϴ�.
void AActor_SolaseadoPhoton::SendCostumeParts(FString Parts)
{
	FString str = "costume";
	InputCharacterInfo(str, Parts);
	SendPlayerInfo();
}


