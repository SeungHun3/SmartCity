// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Struct_Customizing.h"
#include "GameFramework/Pawn.h"
#include "Pawn_Player.generated.h"

//�÷��̾��� �ִϸ��̼� ���� enum
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

 
//�Է¹��� �̵� Ŀ�ǵ忡 ���� ���� ������
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
	// ���� ��ȭ 
	UPROPERTY(BlueprintReadWrite, Category = "Playfab")
		FString VirtualCoin;
	UPROPERTY(BlueprintReadWrite, Category = "Playfab")
		class UActorComponent_PlayfabStore* BP_ActorComponent_Playfab;
	//���� �÷��̾�����
	bool bLocal = false;

	//���� �ο���
	uint8 ParticipantClient;

	// ĳ���� ���̷��� ����
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

	// �ִϸ��̼� ���¼���
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		enum_PlayerAnimationState eAnimationState = enum_PlayerAnimationState::Idle;
	
	
	// ���� ��ġ��
	UPROPERTY(BlueprintReadWrite)
		FVector vPrePos;
	// ���� ���ӵ���
	UPROPERTY(BlueprintReadWrite)
		FVector vVelocity;

	//FPS ����
	float FPStimer = 0.0;
	UPROPERTY(BlueprintReadWrite)
	int CountFPS = 0;//FPS �����
	int tickFPS = 0;

	//�����׽�Ʈ�� ����
	UPROPERTY(BlueprintReadWrite)
	int32 PhotonTimer=0;


	//�Է� �̺�Ʈ Forward, Right
	UPROPERTY(BlueprintReadWrite)
	float fForward = 0.0f;
	UPROPERTY(BlueprintReadWrite)
	float fRight = 0.0f;
	//�Է� �̺�Ʈ Forward, Right
	UPROPERTY(BlueprintReadWrite)
	enum_InputPlayer PrevForward= enum_InputPlayer::ForwardStop;
	UPROPERTY(BlueprintReadWrite)
	enum_InputPlayer PrevRight = enum_InputPlayer::RightStop;


	//�Է� ȸ��
	UPROPERTY(BlueprintReadWrite)
	float fRotationX = 0.0f;

	// ���� �̵� Ÿ�̸�
	UPROPERTY(BlueprintReadWrite)
	float lerpMoveTimer = 0.0f;
	UPROPERTY(BlueprintReadWrite)
		float lerpRotationTimer = 0.0f;

	//���� �̵� ��ǥ
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
	//����

	//���濡�� ���� �ڽ�Ƭ �����͸� �Է¹޴´�.
	UFUNCTION(BlueprintCallable)
	void SetCostumeArray(const TArray<FString>& ITemIDs);

	/////////////////////////////////////////////////////////////////////////////////
	// Player Playfab Data Binding 
	
	// ĳ���� ���� ���ο� ���� ���� ������
	UFUNCTION(BlueprintImplementableEvent)
		void InitPlayFabUserTitleData(bool bChecker);
	// ĳ���� ���� �г��� Ȯ��
	UFUNCTION(BlueprintImplementableEvent)
		void CustomizingWidgetNameChecker(bool bChecker);
	// ĳ���� ���� �г��� ȭ�� + ��Ģ�� 
	UFUNCTION(BlueprintImplementableEvent)
		void updateDisplayNameEvent(bool bChecker);
	// �κ��丮 & ���� ���� ������Ʈ 
	UFUNCTION(BlueprintImplementableEvent)
		// void updateInventoryCoin(const FString& coin);
		// void updateInventoryCoin();
		void updateInventory();

	// �ΰ��� ���� �ε� ī��Ʈ ó�� _ Test
	UFUNCTION(BlueprintImplementableEvent)
		void Check_getIngameLoadingCount();

	// ĳ���� ���� �Ϸ� �� ���� ó��
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_CreateCharacter();
	
	/////////////////////////////////////////////
	///////����, ������ ���� ������Ʈ
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateStore(const TArray<FITemInfo>& ShopDatas);
	UFUNCTION(BlueprintImplementableEvent)
		void CoinUpdate(int coin);


	//���濡 �÷����� ������ ����ȭ = PlayFab�� �ִ� userdata�޾ƿͼ� InstanceID��Ī ���� ITemID�޾ƿ���
	UFUNCTION(BlueprintCallable)
		TArray<FString> UploadPlayer();

	// �κ��丮 ������ ����ϱ�
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_UpdateEquipmentItem(const FString& InstanceID);
	// ������ ���� �� �κ��丮 ������Ʈ.
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_AddInventoryItem(FItemproperty item);
	
	/////////////////////////////////////////////
	// �������� �ο��� üũ
	void AddClentPlayerCount();
	void RemoveClentPlayerCount();




	////////// �޽����� �����Լ�
	////���̷��� �޽� �����Լ�
	UFUNCTION(BlueprintCallable)
	void ChangeMesh(const FString& ClassName,class USkeletalMesh* Mesh);

	// Ŀ���ҽ��۽� ����Ʈ �޽����� �Լ� == ������ ���̺� �޾ƿͼ� ������ ù��°���� �޽��� �����
	UFUNCTION(BlueprintCallable)
	void BeginDefalutMesh();

	// �޽� ����� ���� ����ȭ
	UFUNCTION(BlueprintCallable)
	void ChangeProperty(const FString& ITemID);


	// Zoom_In_out
	UFUNCTION(BlueprintCallable)
		float ZoomFunction(bool isZoomIn, float zoomAxis = 10.0f);



	//////////////////////////////////////////////////////////////
	// �ִϸ��̼� ����
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



	// �޽� ����� ���� ����ȭ
	UFUNCTION(BlueprintCallable)
	bool InputMoveCommand(const enum_InputPlayer& _Command);

	// �̵�
	UFUNCTION(BlueprintImplementableEvent)
		void Pawn_Move(FVector Loc);

	UFUNCTION(BlueprintImplementableEvent)
		void Pawn_StopMove();

	UFUNCTION(BlueprintImplementableEvent)
		void Pawn_StopMoveFinish(FVector Loc);
};
