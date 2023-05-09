// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn_Player.h"
#include "PhotonListner_Solaseado.h"
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

	UPROPERTY()
		TArray<class APawn_Player*> PlayerList;
	// ���� �÷��̾� ĳ����
	UPROPERTY(EditAnywhere)
		TSubclassOf<class APawn_Player> targetCharacter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ������Ʈ �ֱ� 100ms
	unsigned long lastUpdateTime;

	// ���� Ŭ���̾�Ʈ
	ExitGames::LoadBalancing::Client* m_pClient;
	PhotonListner_Solaseado* m_pListener;

	// ĳ���� ��ġ ���� ������Ʈ
	void movePlayer(FVector Loc);
	void movePlayer(int vx, int vy, int vz);
	void movePlayerRotation(float fZ);

	// ���� ��ġ
	FVector CurrentLocation = FVector::ZeroVector;
	float CurrentRotation = 0.f;

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

	// ĳ���� �ִϸ��̼� 
	UFUNCTION(BlueprintCallable)
		void setPlayerAnimationData(uint8 anim);
	// ���� ���� ���� 
	UFUNCTION(BlueprintCallable)
		void ConnectLogin(const FString& username);
	// ä�� , ���� �޼��� ���
	UFUNCTION(BlueprintCallable)
		void SendTextMessage(const FString& Message, const FString& type);
	// Blueprint Ros Actor Spawn 
	UFUNCTION(BlueprintImplementableEvent)
		void ConnectRosActor();


	// ���� ������, ���� ����
	void ReconnectMessage();
	void PhotonDisconnect();

	// ���� ���� ���� ���� 
	virtual void setRegion() override;

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
		void InputCharacterInfo(FString _key, int _value);
	UFUNCTION(BlueprintCallable)
		void SendPlayerInfo();
	UFUNCTION(BlueprintImplementableEvent)
		void ConnectPhotonCHat();

	//����Ƽ�� �̺�Ʈ�� c++�� �������Ʈ ��� �����ؼ� ����
	//���� �������� ������Ʈ ���� �ڽ�Ƭ ������ ó�����ִ� �Լ�
	UFUNCTION(BlueprintNativeEvent)
		void SetCustomCostume(int playerNr, const TArray<FCostume>& arrayCostume);
	virtual void SetCustomCostume_Implementation(int playerNr, const TArray<FCostume>& arrayCostume);

protected:
	float PlayerHeight = 266.f;
	// ������
	// UFUNCTION(BlueprintCallable)
	// void SendPlayerInfo();

	//�׽�Ʈ ���̿� ���� ���� ����
	UFUNCTION(BlueprintCallable)
		void DummyConnectLogin(const FString& username, APawn_Player* dummy);

	virtual void updateLocalPlayerPosion() override;
};
