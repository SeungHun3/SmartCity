// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Photon/SH_PhotonListener.h"
#include "GameFramework/Actor.h"
#include "Actor_Photon.generated.h"

UENUM(BlueprintType)
enum class Enum_TextType : uint8
{
	Normal = 0,
	World,		// 10sec
	infinite,	// infinite
	Custom,
};

UCLASS()
class TEST_LIDARSAMPLE_API AActor_Photon : public AActor , public SH_PhotonBasic
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_Photon();
	virtual ~AActor_Photon();

	// ���� ���� ���� ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SH_Photon")
		FString serverAddress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SH_Photon")
		FString AppID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SH_Photon")
		FString appVersion = FString("1.2.0");

	// ���� ������ �÷��̾� ����
	UPROPERTY()
		TArray<class ACharacter_Player*> PlayerList;
	// ���� �÷��̾� ĳ����
	UPROPERTY(EditAnywhere)
		TSubclassOf<ACharacter_Player> targetCharacter;

	// ���� LiDAR Socket , Roslib Client 
	//UPROPERTY(EditAnywhere)
	//	TSubclassOf<class AActor_RosActor> SpawnRosActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ������Ʈ �ֱ� 100ms
	unsigned long lastUpdateTime;

	// ���� Ŭ���̾�Ʈ
	ExitGames::LoadBalancing::Client* m_pClient;
	SH_PhotonListener* m_pListener;

	// ĳ���� ��ġ ���� ������Ʈ
	void movePlayer(FVector Loc);
	void movePlayer(int vx, int vy, int vz);
	void movePlayerRotation(float fZ);

	// ���� ��ġ
	FVector CurrentLocation = FVector::ZeroVector;
	float CurrentRotation = 0.f;

	// �÷��̾� ����
	class ACharacter_Player* LocalPlayer;

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
};
