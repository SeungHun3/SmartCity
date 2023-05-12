// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_Player.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameInstance_Solaseado.h"
#include "ActorComponent_PlayfabStore.h"

// Sets default values
APawn_Player::APawn_Player()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// ���̷��� ������Ʈ ��������
	Root = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	RootComponent = Root;

	Body = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Root);

	Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	Head->SetupAttachment(Body);

	Hair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair"));
	Hair->SetupAttachment(Head);

	Face = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Face"));
	Face->SetupAttachment(Head);

	Hand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hand"));
	Hand->SetupAttachment(Body);

	//Body->SetVisibility(false, true);
	// child���� �Ǳ� ���� ȣ���ع����� ���������� visible���� ����� ��

	// ���̷��� �޽� ���� visible����ó��
	
	TArray<USkeletalMeshComponent*> PartsMeshes;
	GetComponents<USkeletalMeshComponent>(PartsMeshes);
	for (auto Mesh : PartsMeshes)
	{
		Mesh->SetVisibility(false);
	}
	bUseControllerRotationYaw = true;
	//test
	//Body->SetVisibility(false);
}

// Called when the game starts or when spawned
void APawn_Player::BeginPlay()
{
	Super::BeginPlay();
	Body->SetVisibility(true, true);
}

// Called every frame
void APawn_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


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
			UE_LOG(LogTemp, Log, TEXT("// SetCostumeArray : Error!!  Not Found Item!!!"));
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
					UE_LOG(LogTemp, Log, TEXT("// instanceID : %s , ItemID : %s "), *it, *propertys.ItemId);
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
		//UE_LOG(LogTemp, Log, TEXT("// Name :  %s "), *str);
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
		UE_LOG(LogTemp, Log, TEXT("// Nodata !!!!"));
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
}
