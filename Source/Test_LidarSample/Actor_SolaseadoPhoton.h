// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn_Player.h"
#include "PhotonListner_Solaseado.h"
#include"ActorComponent_Playfab.h"
#include "GameFramework/Actor.h"
#include "Actor_SolaseadoPhoton.generated.h"


//플레이어의 애니메이션 상태 enum
/*
Pawn_PLayer.h에 갱신예정
*/
//UENUM(BlueprintType)
//enum class enum_PlayerAnimationState : uint8
//{
//	Error = 0,
//	Idle,
//	Walk,
//	Run,
//	Sleep,
//	Death,
//};

UENUM(BlueprintType)
enum class Enum_TextType3 : uint8
{
	Normal = 0,
	World,		// 10sec
	infinite,	// infinite
	Custom,
};

//임시로 만들어진 거라 추후 변경예정
//아바타 타입
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

//아바타 파츠 정보 구조체
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
	
	// 포톤 서버 접속 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SH_Photon")
		FString serverAddress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SH_Photon")
		FString AppID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SH_Photon")
		FString appVersion = FString("1.2.0");

	//플레이어 리스트
	UPROPERTY()
		TArray<class APawn_Player*> PlayerList;
	// 스폰 플레이어 캐릭터
	UPROPERTY(EditAnywhere)
		TSubclassOf<class APawn_Player> targetCharacter;

	// 룸 리스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<FString> RoomList;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 업데이트 주기 100ms
	unsigned long lastUpdateTime;

	// 포톤 클라이언트
	ExitGames::LoadBalancing::Client* m_pClient;
	PhotonListner_Solaseado* m_pListener;

	// 캐릭터 위치 포톤 업데이트
	UFUNCTION(BlueprintCallable)
	void movePlayer(FVector Loc);
	void movePlayer(int vx, int vy, int vz);
	void movePlayerRotation(float fZ);
	

	// 현재 입장 방 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString CurrentRoomName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 CurrentPeople;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 MaxPeople;

	// 이전 위치
	FVector CurrentLocation = FVector::ZeroVector;
	float CurrentRotation = 0.f;

	FVector ForwardVector = FVector::ZeroVector;

	// 플레이어 유저
	class APawn_Player* LocalPlayer;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 진행중인 방 이벤트. 0 없음 , 1~ 특정 이벤트
	UFUNCTION(BlueprintCallable)
		void ChangeRoomEventProperty(uint8 Ev);
	UFUNCTION(BlueprintCallable)
		void ChangeEventPause(bool ev);

	// 캐릭터 애니메이션 세팅
	UFUNCTION(BlueprintCallable)
		void setPlayerAnimationData(enum_PlayerAnimationState anim);
	// 포톤 서버 접속 
	UFUNCTION(BlueprintCallable)
		void ConnectLogin(const FString& username);
	// 채팅 , 월드 메세지 출력
	UFUNCTION(BlueprintCallable)
		void SendTextMessage(const FString& Message, const FString& type);
	// Blueprint Ros Actor Spawn 
	UFUNCTION(BlueprintImplementableEvent)
		void ConnectRosActor();

	// 캐릭터 Forward
	UFUNCTION(BlueprintCallable)
	void movePlayerXY(float fX, float fY);

	// 포톤 재접속, 접속 해제
	void ReconnectMessage();
	void PhotonDisconnect();

	// 포톤 지역 설정 변경 
	virtual void setRegion() override;

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
	virtual void GetMovePlayerXYandLeryXY(int playerNr, float fX, float fY, float lerpX, float lerpY) override;

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
	// 캐릭터 정보 데이터(아바타...) 추가 함수
	UFUNCTION(BlueprintCallable)
		void InputCharacterInfo(FString _key, FString _value);
	UFUNCTION(BlueprintCallable)
		void SendPlayerInfo();
	UFUNCTION(BlueprintImplementableEvent)
		void ConnectPhotonCHat();

	//방에 들어가기 전에 playfab에서 받은 플레이어 데이터를 채워주기
	UFUNCTION(BlueprintNativeEvent)
		void InitPlayerData();
	virtual void InitPlayerData_Implementation();

	//애니메이션 상태 데이터를 보내주는 함수
	UFUNCTION(BlueprintCallable)
		void SendCostumeParts(FString Parts);

	//애니메이션 상태 데이터를 보내주는 함수
	UFUNCTION(BlueprintCallable)
		void InputAnimationState(enum_PlayerAnimationState _State);


protected:
	float PlayerHeight = 266.f;

	//플레이어 코스튬 개수
	int DataCount = 0;

	//이동 동기화에 쓰일 변수
	//0.1초 동안 이동 오차 값을 보간하여 이동해준다.
	float lerpTimer = 0.1f;

	//테스트 더미용 포톤 서버 접속
	UFUNCTION(BlueprintCallable)
		void DummyConnectLogin(const FString& username, APawn_Player* dummy);

	virtual void updateLocalPlayerPosion() override;

};
