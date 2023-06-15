// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_Player.h"
#include "GameInstance_Solaseado.h"
#include "GameModeBase_Solaseado.h"


#include "GenericPlatform/GenericPlatformMisc.h"
#include "GameFramework/Actor.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMesh.h"
#include "ActorComponent_PlayfabStore.h"
#include "Actor_SolaseadoPhoton.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
APawn_Player::APawn_Player()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// ���̷��� ������Ʈ ��������
	Root = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	RootComponent = Root;
	Root->SetRelativeTransform(FTransform(FQuat(0.0f, 0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)));
	Body = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Root);

	Hair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair"));
	Hair->SetupAttachment(Body);

	Top = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Top"));
	Top->SetupAttachment(Body);

	Bottoms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bottoms"));
	Bottoms->SetupAttachment(Body);

	Shoes = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shoes"));
	Shoes->SetupAttachment(Body);
	
	Body->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));



	//�ִϸ��̼� ����
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimationBPClass(TEXT("/Game/Project/Skeleton/anim/anim_body/ABP_Player.ABP_Player_C"));//����
	//static ConstructorHelpers::FClassFinder<UAnimInstance> AnimationBPClass(TEXT("/Game/Project/Skeleton/anim/anim_body/ABP_Player.ABP_Player_C"));	// ����
	if (AnimationBPClass.Class)	// ���� ���н� beginPlay�� ���� üũ �� set����
	{
		// Set the Animation Blueprint
		Body->SetAnimInstanceClass(AnimationBPClass.Class);
	}


	//Body->SetVisibility(false, true);
	// child���� �Ǳ� ���� ȣ���ع����� ���������� visible���� ����� ��

	// ���̷��� �޽� ���� visible����ó��
	
	TArray<USkeletalMeshComponent*> PartsMeshes;
	GetComponents<USkeletalMeshComponent>(PartsMeshes);
	for (auto Mesh : PartsMeshes)
	{
		Mesh->SetVisibility(false);
	}

	//Pawn�� ȸ���� Yaw �� controller�� ������ ���� = false
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	
}

// Called when the game starts or when spawned
void APawn_Player::BeginPlay()
{
	Super::BeginPlay();

	// �α� ��� �׽�Ʈ
	//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT(" // PawnBeginPlay")), true, true, FColor::Green, 10.0f);
}

// Called every frame
void APawn_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//tick FPS ����
	FPStimer += DeltaTime;
	++tickFPS;
	if (FPStimer >= 1.0)
	{
		FPStimer = 0.0f;
		CountFPS = tickFPS;
		tickFPS = 0;
	}

}

// Called to bind functionality to input
void APawn_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FString APawn_Player::getLoginDeviceID()
{
	return FGenericPlatformMisc::GetMacAddressString();
}


// ���濡�� AddPlayer�� �޾ƿ� Property���� ITemIDs�� �޾ƿ�
// =====>>>>>>ĳ���� Mesh���� ���ִ� �Լ�
void APawn_Player::SetCostumeArray(const TArray<FString>& ITemIDs)
{
	//Instance�� �ִ� FindITem �Լ� �̿��ϱ�
	UGameInstance_Solaseado* MyInstance = Cast<UGameInstance_Solaseado>(GetGameInstance());

	for (auto ItemID : ITemIDs)
	{
		FCustomizing_Struct CustomStruct = MyInstance->Find_ITem(ItemID);
		if (!CustomStruct.ItemID.IsEmpty())
		{
			ChangeMesh(CustomStruct.ClassName, CustomStruct.Mesh);
		}
		else
		{
			//UE_LOG(LogTemp, Log, TEXT("// SetCostumeArray : Error!!  Not Found Item!!!"));
		}
	}
	

}

// UserTitleData = InstanceID  =>> ItemID�迭�� ����ִ� �Լ�
TArray<FString> APawn_Player::UploadPlayer()
{
	TArray<FString> ItemIDs;
	if (BP_ActorComponent_Playfab)
	{
		TArray<FString> TitleKeys;
		TArray<FString> TitleValue;

		// TitleKeys�� ��� Ű ä����
		BP_ActorComponent_Playfab->UserTitleData.GetKeys(TitleKeys);

		// TitleValue �� �� ����
		for (auto keys : TitleKeys)
		{
			TitleValue.Push(*BP_ActorComponent_Playfab->UserTitleData.Find(keys));
			
		}
		// �ϴ� ��� value �̾Ƴ���
		// ������ ��
		for (auto propertys : BP_ActorComponent_Playfab->getInventoryItemList())
		{
			for (auto it : TitleValue)
			{
				if (it == propertys.ItemInstanceId)
				{
					// ���� instanceID���� üũ �� �迭�� ����
					//UE_LOG(LogTemp, Log, TEXT("// instanceID : %s , ItemID : %s "), *it, *propertys.ItemId);
					ItemIDs.Push(propertys.ItemId);
				}
			}

		}
	}

	return ItemIDs;
}




void APawn_Player::AddClentPlayerCount()
{
	++ParticipantClient;
}

void APawn_Player::RemoveClentPlayerCount()
{
	--ParticipantClient;
}

// �޽� ���� ��Ű�� �Լ� 
void APawn_Player::ChangeMesh(const FString& ClassName, USkeletalMesh* Mesh)
{
	TArray<USkeletalMeshComponent*> MeshComponents;
	GetComponents<USkeletalMeshComponent>(MeshComponents);
	for (auto Parts : MeshComponents)
	{
		FString PartsName;
		PartsName = Parts->GetName();
		//UE_LOG(LogTemp, Log, TEXT("// Name :  %s "), *PartsName);
		if (PartsName == ClassName)
		{
			Parts->SetSkeletalMesh(Mesh);
		}
	}
}

// ó�����۽� PlayFab�����Ͱ� ���� ��� default�� �޽� ä���ִ� �Լ�
void APawn_Player::BeginDefalutMesh()
{

	//Instance�� �ִ� ������ ���̺� ���������
	UGameInstance_Solaseado* MyInstance = Cast<UGameInstance_Solaseado>(GetGameInstance());
	TArray<UDataTable*>InstanceDataTables = MyInstance->GetDataTables();


	if (!InstanceDataTables.IsValidIndex(0)) // ������ ���̺��� ���ٸ� Ż��
	{
		UE_LOG(LogTemp, Warning, TEXT("// Nodata !!!!"));
		return;
	}
	// ������ ���̺��� �ִٸ�
	for (auto Table : InstanceDataTables)
	{
		//�޾ƿ� ������ ���̺� �迭���� �� ���̸� String "1" �� �����͸� �޾ƿ���
		FCustomizing_Struct CustomStruct = *Table->FindRow<FCustomizing_Struct>("1", "");
		if (!CustomStruct.ItemID.IsEmpty())
		{
			//�����Ͱ� ����� ������ Ȯ�� �� changeMesh �Լ� ����
			ChangeMesh(CustomStruct.ClassName, CustomStruct.Mesh);
		}
	}
	// ������ ���� ����� �������ѱ�
	Body->SetVisibility(true, true);
	
}

void APawn_Player::ChangeProperty(const FString& ITemID)
{
	AGameModeBase_Solaseado* GM_Solaseado = Cast<AGameModeBase_Solaseado>(GetWorld()->GetAuthGameMode());
	//GM_Solaseado->PhotonCloud->InputCharacterInfo("Pawn", ITemID); 
	GM_Solaseado->PhotonCloud->SendCostumeParts(ITemID);
	 
}

float APawn_Player::ZoomFunction(bool isZoomIn, float zoomAxis)
{
	if (!BP_SpringArm)
	{
		return BP_SpringArm->TargetArmLength;
	}


	float zoomValue = BP_SpringArm->TargetArmLength;
	// ���� : -Axis , �ܾƿ�  + Axis; 
	isZoomIn ? zoomValue -= zoomAxis : zoomValue += zoomAxis;
	

	// ����� ���� SpringArm�� ����
	BP_SpringArm->TargetArmLength = FMath::Clamp(zoomValue, 50.0f, 500.0f);


	return BP_SpringArm->TargetArmLength;
}



///  CostumePawn    //////////////////////////
///////////////////////////////////////////
bool APawn_Player::Bind_Init_Implementation(UWidget_CustomizingTab* WB_Customizing_Tab)
{
	return false;
}

bool APawn_Player::Select_Position_Implementation(int ClassName)
{
	return false;
}

void APawn_Player::Change_Scene_Implementation(int TabNumber)
{
	
}



///////////////////////////////////////////
///////////////////////////////////////////
//�������� ���Ź��� �Է¿� ���� ���¸� �� �Լ����� ó�����ش�.\
//�ִϸ��̼� ���°��� ��ȭ�� ���� ��� true�� ��ȯ�ϰ� �ƴҰ�� false�� ��ȯ�մϴ�.
bool APawn_Player::InputMoveCommand(const enum_InputPlayer& _Command)
{
	switch ((enum_InputPlayer)_Command)
	{
	case enum_InputPlayer::Error:
		return false;
	case enum_InputPlayer::RightStop:
		fRight = 0.0f;
		break;
	case enum_InputPlayer::Right:
		fRight = 1.0f;
		break;
	case enum_InputPlayer::Left:
		fRight = -1.0f;
		break;
	case enum_InputPlayer::ForwardStop:
		fForward = 0.0f;
		break;
	case enum_InputPlayer::Forward:
		fForward = 1.0f;
		break;
	case enum_InputPlayer::Back:
		fForward = -1.0f;
		break;
	default:
		break;
	}

	//UE_LOG(LogTemp, Log, TEXT("// InputMoveCommand PlayerNr: %d, Command ::%d"), PlayerNr, _Command);
	

	//�ִϸ��̼� ����
	// �ϳ��� ������Ʈ���� ����Ǵ� ����, ������Ʈ �߰��� �߰�����
	if ((fForward ==0 && fRight == 0) && eAnimationState == enum_PlayerAnimationState::Walk)
	{
		eAnimationState = enum_PlayerAnimationState::Idle;
		Change_Anim(enum_PlayerAnimationState::Idle); 

		return true;
	}
	else if ((fForward || fRight) && eAnimationState == enum_PlayerAnimationState::Idle)
	{
		eAnimationState = enum_PlayerAnimationState::Walk;
		Change_Anim(enum_PlayerAnimationState::Walk);

		return true;
	}

	return false;
}
