// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawn_Player.h"
#include "Photon/PhotonListner_Solaseado.h"
#include "ActorComponent_Playfab.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<class APawn_Player*> PlayerList;
	// 스폰 플레이어 캐릭터
	UPROPERTY(EditAnywhere)
		TSubclassOf<class APawn_Player> targetCharacter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 업데이트 주기 100ms
	unsigned long lastUpdateTime;

	// 포톤 클라이언트
	ExitGames::LoadBalancing::Client* m_pClient;
	PhotonListner_Solaseado* m_pListener;


	// 현재 입장 방 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString CurrentRoomName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 CurrentPeople;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		uint8 MaxPeople;

	// 플레이어 유저
	class APawn_Player* LocalPlayer;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 진행중인 방 이벤트. 0 없음 , 1~ 특정 이벤트
	UFUNCTION(BlueprintCallable)
		void ChangeRoomEventProperty(uint8 Ev);

	// 캐릭터 애니메이션 세팅
	UFUNCTION(BlueprintCallable)
		void setPlayerAnimationData(enum_PlayerAnimationState anim);
	// 포톤 서버 접속 
	UFUNCTION(BlueprintCallable)
		void ConnectLogin(const FString& username);
	
	// Blueprint Ros Actor Spawn 
	UFUNCTION(BlueprintImplementableEvent)
		void ConnectRosActor();

	//////////////////////////////////////////////// 
	// Blueprint Widget Binding Func
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_RemovePlayer(int PlayerNr);
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_AddPlayer(int PlayerNr, const FString& name);
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_ResetPlayer();

	//시작시 위치 보내주는 용도
	UFUNCTION(BlueprintCallable)
		void MoveAndRotation(FVector Loc, int vYaw);
	
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
	virtual void GetMoveAndRotation(int playerNr, int vX, int vY, int vZ, int vYaw) override;
	
	//사용자 애니메이션 정보 업데이트
	virtual void GetPlayerAnim(int playerNr, int Anim) override;
	// Connect
	virtual void ConnectComplete(void) override;

	virtual void CreateChannelComplete(const ExitGames::Common::JString& map, const ExitGames::Common::JString& channel) override;
	virtual void CreateRoomComplete(const ExitGames::Common::JString& map) override;
	virtual void JoinRoomComplete(const ExitGames::Common::JString& map, const ExitGames::Common::JString& channel) override;
	virtual void JoinOrCreateComplete(const FString& RoomFullName) override;

	virtual void LeaveRoomComplete(void) override;
	virtual void CurrentRoomInfo(const ExitGames::Common::JString& name, nByte Count, nByte MaxCount) override;

	//친구 신청 받기
	virtual void RecvFriendRequest(int PlayerNum, const FString& FriendPlayFabID) override;
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

	// 친구 신청
	UFUNCTION(BlueprintCallable)
	void SendFriendInfo(int Target, FString PlayfabID);

	
protected:
	//플레이어 코스튬 개수
	int DataCount = 0;
////////////////////////////////////
/////////// 채널 ///////////////////   cpp 140
protected:
	ExitGames::LoadBalancing::Client*	dummy_pClient;
	PhotonListner_Solaseado*			dummy_pListener;

	bool IsChangingRoom = false;
public:
	
	// 룸 리스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<int, int> ChannelList;

	// 방변경
	UFUNCTION(BlueprintCallable)
		void ChangeRoom(int Number);
	
	UFUNCTION(BlueprintCallable)
		void OpenDummy();
	
	UFUNCTION(BlueprintCallable)
		virtual	void UpdateRoomList(const TMap<int, int>& Channel_Count) override;

	UFUNCTION(BlueprintCallable)
		void CloseDummy();

	UFUNCTION(BlueprintImplementableEvent)
		void ChangeViewCount(const TMap<int, int>& Channel_Count ,const TArray<FString>& PlayerNames);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		int GetMyChannelNumber();

////////////////////////////////////////////////채널 끝

	
////////////////////////////////////////////////
////이동로직////////////////////////////////////////
	UFUNCTION(BlueprintCallable)
		void PhotonMove(FVector Loc);
	UFUNCTION(BlueprintCallable)
		void PhotonStop();
	UFUNCTION(BlueprintCallable)
		void PhotonFinish(FVector Loc);
	
	virtual void UpdateMove(int PlayerNum, FVector Loc) override;
	virtual void UpdateStop(int PlayerNum) override;
	virtual void UpdateStopFinished(int PlayerNum, FVector Loc) override;

////////////////////////////////////////////////이동 끝


//NPC 로직///////////
public:
	//NPC 리스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<class APawn_NPC*> NPCList;
	// 스폰 NPC
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APawn_NPC> targetNPC;

	//NPC 정보 데이터테이블
	UDataTable* CustomNPCDataTable;
	
	virtual void InitNpc() override;
	virtual void AddNpc(const FString& NpcId, FVector Loc, FVector Rot, FVector Scale) override;
	virtual void ClearNpc() override;

	//NPC 이동
	UFUNCTION(BlueprintCallable)
	void NPCMove(const APawn_NPC *pawnNPC, FVector NpcLocation);

//NPC 로직 끝///////////
};