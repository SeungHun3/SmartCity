// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Struct_Customizing.h"
#include "GameFramework/Pawn.h"
#include "Pawn_Player.generated.h"

//플레이어의 애니메이션 상태 enum
UENUM(BlueprintType)
enum class enum_PlayerAnimationState : uint8
{
	Error = 0,
	Idle,
	Walk,
	Run,
	Sleep,
	Death,
};

 
//입력받은 이동 커맨드에 대한 상태 열거형
UENUM(BlueprintType)
enum class enum_InputPlayer : uint8
{
	Error = 0,
	RightStop,
	Right,
	Left,
	ForwardStop,
	Forward,
	Back,
};



UCLASS()
class TEST_LIDARSAMPLE_API APawn_Player : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawn_Player();

	UPROPERTY(BlueprintReadWrite)
		bool bWalk = false;
	UPROPERTY(BlueprintReadWrite, Category = "Photon")
		int PlayerNr;
	UPROPERTY(BlueprintReadWrite, Category = "Photon")
		FString PlayerName;
	UPROPERTY(BlueprintReadWrite)
		bool isInRoom = false;
	UPROPERTY(BlueprintReadWrite)
		bool isInLoby = false;

	UPROPERTY(BlueprintReadOnly, Category = "Playfab")
		int Test_LoadingCount;
	// 가상 통화 
	UPROPERTY(BlueprintReadWrite, Category = "Playfab")
		FString VirtualCoin;
	UPROPERTY(BlueprintReadWrite, Category = "Playfab")
		class UActorComponent_PlayfabStore* BP_ActorComponent_Playfab;
	//로컬 플레이어인지
	bool bLocal = false;

	//참가 인원수
	uint8 ParticipantClient;

	// 캐릭터 스켈레톤 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UCapsuleComponent* Root;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent* Body;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent* Hair;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent* Top;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent* Bottoms;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent* Shoes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USpringArmComponent* BP_SpringArm;

	// 애니매이션 상태설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		enum_PlayerAnimationState eAnimationState = enum_PlayerAnimationState::Idle;
	
	
	// 이전 위치값
	UPROPERTY(BlueprintReadWrite)
		FVector vPrePos;
	// 받은 가속도값
	UPROPERTY(BlueprintReadWrite)
		FVector vVelocity;

	//FPS 측정
	float FPStimer = 0.0;
	UPROPERTY(BlueprintReadWrite)
	int CountFPS = 0;//FPS 결과값
	int tickFPS = 0;

	//지연테스트용 변수
	UPROPERTY(BlueprintReadWrite)
	int32 PhotonTimer=0;


	//입력 이벤트 Forward, Right
	UPROPERTY(BlueprintReadWrite)
	float fForward = 0.0f;
	UPROPERTY(BlueprintReadWrite)
	float fRight = 0.0f;
	//입력 이벤트 Forward, Right
	UPROPERTY(BlueprintReadWrite)
	enum_InputPlayer PrevForward= enum_InputPlayer::ForwardStop;
	UPROPERTY(BlueprintReadWrite)
	enum_InputPlayer PrevRight = enum_InputPlayer::RightStop;


	//입력 회전
	UPROPERTY(BlueprintReadWrite)
	float fRotationX = 0.0f;

	// 보간 이동 타이머
	UPROPERTY(BlueprintReadWrite)
	float lerpMoveTimer = 0.0f;
	UPROPERTY(BlueprintReadWrite)
		float lerpRotationTimer = 0.0f;

	//보간 이동 좌표
	UPROPERTY(BlueprintReadWrite)
		float fLerpMoveX = 0.0f;
	UPROPERTY(BlueprintReadWrite)
		float fLerpMoveY = 0.0f;
	UPROPERTY(BlueprintReadWrite)
		float fLerpMoveZ = 0.0f;
	UPROPERTY(BlueprintReadWrite)
		float fLerpRotationX = 0.0f;


	UPROPERTY(BlueprintReadWrite)
	bool bRotation = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		FString getLoginDeviceID();
	////////////////////////////////////////////////////////
	//포톤

	//포톤에서 받은 코스튬 데이터를 입력받는다.
	UFUNCTION(BlueprintCallable)
	void SetCostumeArray(const TArray<FString>& ITemIDs);

	/////////////////////////////////////////////////////////////////////////////////
	// Player Playfab Data Binding 
	
	// 캐릭터 생성 여부에 따른 로직 변경점
	UFUNCTION(BlueprintImplementableEvent)
		void InitPlayFabUserTitleData(bool bChecker);
	// 캐릭터 생성 닉네임 확인
	UFUNCTION(BlueprintImplementableEvent)
		void CustomizingWidgetNameChecker(bool bChecker);
	// 캐릭터 생성 닉네임 화인 + 금칙어 
	UFUNCTION(BlueprintImplementableEvent)
		void updateDisplayNameEvent(bool bChecker);
	// 인벤토리 & 코인 정보 업데이트 
	UFUNCTION(BlueprintImplementableEvent)
		// void updateInventoryCoin(const FString& coin);
		// void updateInventoryCoin();
		void updateInventory();

	// 인게임 접속 로딩 카운트 처리 _ Test
	UFUNCTION(BlueprintImplementableEvent)
		void Check_getIngameLoadingCount();

	// 캐릭터 생성 완료 후 로직 처리
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_CreateCharacter();
	
	/////////////////////////////////////////////
	///////상점, 아이템 정보 업데이트
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateStore(const TArray<FITemInfo>& ShopDatas);
	UFUNCTION(BlueprintImplementableEvent)
		void CoinUpdate(int coin);


	//포톤에 플레이팹 데이터 동기화 = PlayFab에 있는 userdata받아와서 InstanceID매칭 시켜 ITemID받아오기
	UFUNCTION(BlueprintCallable)
		TArray<FString> UploadPlayer();

	// 인벤토리 아이템 장비하기
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_UpdateEquipmentItem(const FString& InstanceID);
	// 아이템 구매 후 인벤토리 업데이트.
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_AddInventoryItem(FItemproperty item);
	
	/////////////////////////////////////////////
	// 참가중인 인원수 체크
	void AddClentPlayerCount();
	void RemoveClentPlayerCount();




	////////// 메쉬관련 변경함수
	////스켈레톤 메쉬 변경함수
	UFUNCTION(BlueprintCallable)
	void ChangeMesh(const FString& ClassName,class USkeletalMesh* Mesh);

	// 커스텀시작시 디폴트 메쉬변경 함수 == 데이터 테이블 받아와서 파츠당 첫번째열의 메쉬를 사용함
	UFUNCTION(BlueprintCallable)
	void BeginDefalutMesh();

	// 메쉬 변경시 포톤 동기화
	UFUNCTION(BlueprintCallable)
	void ChangeProperty(const FString& ITemID);


	// Zoom_In_out
	UFUNCTION(BlueprintCallable)
		float ZoomFunction(bool isZoomIn, float zoomAxis = 10.0f);



	//////////////////////////////////////////////////////////////
	// 애니매이션 변경
	UFUNCTION(BlueprintImplementableEvent)
		void Change_Anim(enum_PlayerAnimationState animstate);



	/////////////////////////
	//Costume Pawn Function
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool Bind_Init(class UWidget_CustomizingTab* WB_Customizing_Tab);
	virtual bool Bind_Init_Implementation(class UWidget_CustomizingTab* WB_Customizing_Tab);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool Select_Position(int ClassName);
	virtual bool Select_Position_Implementation(int ClassName);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Change_Scene(int TabNumber);
	virtual void Change_Scene_Implementation(int TabNumber);



	// 메쉬 변경시 포톤 동기화
	UFUNCTION(BlueprintCallable)
	bool InputMoveCommand(const enum_InputPlayer& _Command);

	// 이동
	UFUNCTION(BlueprintImplementableEvent)
		void Pawn_Move(FVector Loc);

	UFUNCTION(BlueprintImplementableEvent)
		void Pawn_StopMove();

	UFUNCTION(BlueprintImplementableEvent)
		void Pawn_StopMoveFinish(FVector Loc);
};
