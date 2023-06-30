// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn_Player.h"
#include "PhotonListner_Solaseado.h"
#include"ActorComponent_Playfab.h"
#include "GameFramework/Actor.h"
#include "Actor_SolaseadoPhoton.generated.h"


UENUM(BlueprintType)
enum class Enum_TextType3 : uint8
{
	Normal = 0,
	World,		// 10sec
	infinite,	// infinite
	Custom,
};

//�ӽ÷� ������� �Ŷ� ���� ���濹��
//�ƹ�Ÿ Ÿ��
UENUM(BlueprintType)
enum class enum_CostumeType : uint8
{
	Top=0,
	Bottom,
	Hair,
	Face,
	Skin,
	Default,
};

//�ƹ�Ÿ ���� ���� ����ü
USTRUCT(BlueprintType)
struct FCostume
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	enum_CostumeType Type= enum_CostumeType::Default;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PartNumber=0;
};


UCLASS()
class TEST_LIDARSAMPLE_API AActor_SolaseadoPhoton : public AActor, public SH_PhotonBasic
{
	GENERATED_BODY()

public:
	//Enum_TextType aa;
	// Sets default values for this actor's properties
	AActor_SolaseadoPhoton();
	virtual ~AActor_SolaseadoPhoton();
	
	// ���� ���� ���� ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SH_Photon")
		FString serverAddress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SH_Photon")
		FString AppID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SH_Photon")
		FString appVersion = FString("1.2.0");

	//�÷��̾� ����Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<class APawn_Player*> PlayerList;
	// ���� �÷��̾� ĳ����
	UPROPERTY(EditAnywhere)
		TSubclassOf<class APawn_Player> targetCharacter;

	// �� ����Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<FString> RoomList;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ������Ʈ �ֱ� 100ms
	unsigned long lastUpdateTime;

	// ���� Ŭ���̾�Ʈ
	ExitGames::LoadBalancing::Client* m_pClient;
	PhotonListner_Solaseado* m_pListener;


	// ĳ���� ��ġ ���� ������Ʈ
	UFUNCTION(BlueprintCallable)
	void movePlayer(FVector Loc);
	void movePlayer(int vx, int vy, int vz);
	UFUNCTION(BlueprintCallable)
	void movePlayerRotation(float fZ);
	

	// ���� ���� �� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString CurrentRoomName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 CurrentPeople;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 MaxPeople;

	// ���� ��ġ
	FVector CurrentLocation = FVector::ZeroVector;
	float CurrentRotation = 0.f;

	FVector ForwardVector = FVector::ZeroVector;

	// �÷��̾� ����
	class APawn_Player* LocalPlayer;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// �������� �� �̺�Ʈ. 0 ���� , 1~ Ư�� �̺�Ʈ
	UFUNCTION(BlueprintCallable)
		void ChangeRoomEventProperty(uint8 Ev);
	UFUNCTION(BlueprintCallable)
		void ChangeEventPause(bool ev);

	// ĳ���� �ִϸ��̼� ����
	UFUNCTION(BlueprintCallable)
		void setPlayerAnimationData(enum_PlayerAnimationState anim);
	// ���� ���� ���� 
	UFUNCTION(BlueprintCallable)
		void ConnectLogin(const FString& username);
	// ä�� , ���� �޼��� ���
	UFUNCTION(BlueprintCallable)
		void SendTextMessage(const FString& Message, const FString& type);
	// Blueprint Ros Actor Spawn 
	UFUNCTION(BlueprintImplementableEvent)
		void ConnectRosActor();


	// Ŀ�ǵ� �Է��� �Ǿ����� ������ �۽�
	UFUNCTION(BlueprintCallable)
	void movePlayerCommand(enum_InputPlayer _Commnad);
	// ĳ���� ȸ�� Yaw
	UFUNCTION(BlueprintCallable)
	void RotationPlayerX(float fX);
	//ȸ�� ������ �Լ�(�̱���, �ʿ�� ����)
	void MovePlayerRotationAndTime(float fX, int time);
	//��ġ ������ �Լ�
	void MovePlayerAndTime(int vX, int vY, int time);
	UFUNCTION(BlueprintCallable)
	void MoveAndRotation(FVector Loc, int vYaw);

	// ���� ������, ���� ����
	void ReconnectMessage();
	void PhotonDisconnect();

	// ���� ���� ���� ���� 
	virtual void setRegion(ExitGames::LoadBalancing::Client* Client) override;

	// ���� ���
	virtual void ErrorCheckMessage(const FString& message, int error) override;

	// ���� ����� �ʱ�ȭ
	virtual void InitPlayers(void);
	// ���� ����� �߰�
	virtual void AddPlayers(int playerNr, const ExitGames::Common::JString& playerName, bool local, const ExitGames::Common::Hashtable& Custom) override;
	// ���� ����� ����
	virtual void RemovePlayer(int playerNr) override;
	// ���� ����� ������Ƽ ������Ʈ
	virtual void updatePlayerProperties(int playerNr, const Hashtable& changes) override;
	// ���� ����� �� ���� ���� // ������Ƽ ������Ʈ
	virtual void updateRoomProperties(const Hashtable& changes) override;

	// ���� ����� ���� ���� ������Ʈ
	virtual void GetMovePlayer(int playerNr, int vX, int vY, int vZ) override;
	virtual void GetMovePlayerRotation(int playerNr, float fX) override;
	virtual void GetPlayerRotationYaw(int playerNr, float fYaw) override;
	virtual void GetMovePlayerCommand(int playerNr, int iCommand) override;
	virtual void GetMovePlayerRotationAndTime(int playerNr, float fX, int time) override;
	virtual void GetMovePlayerAndTime(int playerNr, int vX, int vY, int time) override;
	virtual void GetMoveAndRotation(int playerNr, int vX, int vY, int vZ, int vYaw) override;
	
	//����� �ִϸ��̼� ���� ������Ʈ
	virtual void GetPlayerAnim(int playerNr, int Anim) override;
	// Connect
	virtual void ConnectComplete(void) override;

	virtual void CreateChannelComplete(const ExitGames::Common::JString& map, const ExitGames::Common::JString& channel) override;
	virtual void CreateRoomComplete(const ExitGames::Common::JString& map) override;
	virtual void JoinRoomComplete(const ExitGames::Common::JString& map, const ExitGames::Common::JString& channel) override;
	virtual void JoinOrCreateComplete() override;

	virtual void LeaveRoomComplete(void) override;
	virtual void CurrentRoomInfo(const ExitGames::Common::JString& name, nByte Count, nByte MaxCount) override;

	// Message
	virtual void getTextMessage(int playerNr, const ExitGames::Common::JString& Message, const ExitGames::Common::JString& Type)  override;

	// Quiz Event 
	virtual void getEventPause(bool ev) override;

public:
	// ĳ���� ���� ������(�ƹ�Ÿ...) �߰� �Լ�
	UFUNCTION(BlueprintCallable)
		void InputCharacterInfo(FString _key, FString _value);
	UFUNCTION(BlueprintCallable)
		void SendPlayerInfo();
	UFUNCTION(BlueprintImplementableEvent)
		void ConnectPhotonCHat();

	//�濡 ���� ���� playfab���� ���� �÷��̾� �����͸� ä���ֱ�
	UFUNCTION(BlueprintNativeEvent)
		void InitPlayerData();
	virtual void InitPlayerData_Implementation();

	//�ִϸ��̼� ���� �����͸� �����ִ� �Լ�
	UFUNCTION(BlueprintCallable)
		void SendCostumeParts(FString Parts);

	//�ִϸ��̼� ���� �����͸� �����ִ� �Լ�
	UFUNCTION(BlueprintCallable)
		void InputAnimationState(enum_PlayerAnimationState _State);


protected:
	float PlayerHeight = 266.f;

	//�ӽ� �÷��̾� ���ǵ�
	float moveSpeed;
	//���� �Ÿ� �Ѱ�ġ
	//�� �Ÿ� �̻� �־����� ���� ��ġ ������ ���ش�.
	float lerpDistance;

	//�÷��̾� �ڽ�Ƭ ����
	int DataCount = 0;

	//�̵� ����ȭ�� ���� ����
	//0.1�� ���� �̵� ���� ���� �����Ͽ� �̵����ش�.
	float lerpTimer = 0.1f;
	
	//���� �׽�Ʈ ���� ������ ����ȭ �κ� OnOff
	//false : �̵� ���� ����
	//true : ���� �Ÿ� �־����� �̵��� ��������
	bool bOnTimeMove = true;

	// ������ ������ �ð�
	unsigned long lastMoveTime = 0;
	// �����̱� ������ �ð�
	unsigned long startMoveTime = 0;

	//���� �÷��̾��� �������� �ִ���
	bool bIsMoving = false;




////////////////////////////////////
/////////// ä�� ///////////////////   cpp 140
protected:
	ExitGames::LoadBalancing::Client* dummy_pClient;
	PhotonListner_Solaseado* dummy_pListener;

	int RoomNumber = 0;
	bool IsChangingRoom = false;
public:
	// �溯��
	UFUNCTION(BlueprintCallable)
		void ChangeRoom(int Number);
	
	UFUNCTION(BlueprintCallable)
		void OpenDummy();
	
	UFUNCTION(BlueprintCallable)
		virtual	void UpdateRoomList() override;

	UFUNCTION(BlueprintCallable)
		void CloseDummy();

	UFUNCTION(BlueprintImplementableEvent)
		void ChangeViewCount();

////////////////////////////////////////////////ä�� ��



	virtual void updateLocalPlayerPosion() override;

};
