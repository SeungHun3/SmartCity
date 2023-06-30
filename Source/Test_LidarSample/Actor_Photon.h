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

	// 포톤 서버 접속 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SH_Photon")
		FString serverAddress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SH_Photon")
		FString AppID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SH_Photon")
		FString appVersion = FString("1.2.0");

	// 현재 참가한 플레이어 유저
	UPROPERTY()
		TArray<class ACharacter_Player*> PlayerList;
	// 스폰 플레이어 캐릭터
	UPROPERTY(EditAnywhere)
		TSubclassOf<ACharacter_Player> targetCharacter;

	// 스폰 LiDAR Socket , Roslib Client 
	//UPROPERTY(EditAnywhere)
	//	TSubclassOf<class AActor_RosActor> SpawnRosActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 업데이트 주기 100ms
	unsigned long lastUpdateTime;

	// 포톤 클레이언트
	ExitGames::LoadBalancing::Client* m_pClient;
	SH_PhotonListener* m_pListener;

	// 캐릭터 위치 포톤 업데이트
	void movePlayer(FVector Loc);
	void movePlayer(int vx, int vy, int vz);
	void movePlayerRotation(float fZ);

	// 이전 위치
	FVector CurrentLocation = FVector::ZeroVector;
	float CurrentRotation = 0.f;

	// 플레이어 유저
	class ACharacter_Player* LocalPlayer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 진행중인 방 이벤트. 0 없음 , 1~ 특정 이벤트
	UFUNCTION(BlueprintCallable)
		void ChangeRoomEventProperty(uint8 Ev);
	UFUNCTION(BlueprintCallable)
		void ChangeEventPause(bool ev);

	// 캐릭터 애니메이션 
	UFUNCTION(BlueprintCallable)
		void setPlayerAnimationData(uint8 anim);
	// 포톤 서버 접속 
	UFUNCTION(BlueprintCallable)
		void ConnectLogin(const FString& username);
	// 채팅 , 월드 메세지 출력
	UFUNCTION(BlueprintCallable)
		void SendTextMessage(const FString& Message, const FString& type);
	// Blueprint Ros Actor Spawn 
	UFUNCTION(BlueprintImplementableEvent)
		void ConnectRosActor();
	
	// 포톤 재접속, 접속 해제
	void ReconnectMessage();
	void PhotonDisconnect();

	// 포톤 지역 설정 변경 
	virtual void setRegion(ExitGames::LoadBalancing::Client* Client) override;

	// 에레 출력
	virtual void ErrorCheckMessage(const FString& message, int error) override;

	// 포톤 사용자 초기화
	virtual void InitPlayers(void);
	// 포톤 사용자 추가
	virtual void AddPlayers(int playerNr, const ExitGames::Common::JString& playerName, bool local, const ExitGames::Common::Hashtable& Custom) override;
	// 포톤 사용자 제거
	virtual void RemovePlayer(int playerNr) override;
	// 포톤 사용자 프로퍼티 업데이트
	virtual void updatePlayerProperties(int playerNr, const Hashtable& changes) override;
	// 포톤 사용자 방 정보 변경 // 프로퍼티 업데이트
	virtual void updateRoomProperties(const Hashtable& changes) override;

	// 포톤 사용자 위지 정보 업데이트
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
