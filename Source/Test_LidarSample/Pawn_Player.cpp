// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_Player.h"
#include "GameInstance_Solaseado.h"
#include "GameModeBase_Solaseado.h"


#include "GenericPlatform/GenericPlatformMisc.h"
#include "GameFramework/Actor.h"

#include "GameFramework/SpringArmComponent.h"

#include "ActorComponent_PlayfabStore.h"
#include "Actor_SolaseadoPhoton.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
APawn_Player::APawn_Player()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//애니매이션 설정
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimationBPClass(TEXT("/Game/Project/Skeleton/anim/anim_body/ABP_Player.ABP_Player_C"));//여자
	//static ConstructorHelpers::FClassFinder<UAnimInstance> AnimationBPClass(TEXT("/Game/Project/Skeleton/anim/anim_body/ABP_Player.ABP_Player_C"));	// 남자
	if (AnimationBPClass.Class)	// 남녀 구분시 beginPlay에 남녀 체크 후 set예정
	{
		// Set the Animation Blueprint
		Body->SetAnimInstanceClass(AnimationBPClass.Class);
	}


	//Body->SetVisibility(false, true);
	// child설정 되기 전에 호출해버려서 개별적으로 visible세팅 해줘야 함

	// 스켈레톤 메쉬 전부 visible세팅처리
	
	TArray<USkeletalMeshComponent*> PartsMeshes;
	GetComponents<USkeletalMeshComponent>(PartsMeshes);
	for (auto Mesh : PartsMeshes)
	{
		Mesh->SetVisibility(false);
	}

	//Pawn의 회전값 Yaw 를 controller의 값으로 세팅 = false
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	
}

// Called when the game starts or when spawned
void APawn_Player::BeginPlay()
{
	Super::BeginPlay();

	// 로그 출력 테스트
	//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT(" // PawnBeginPlay")), true, true, FColor::Green, 10.0f);


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


// 포톤에서 AddPlayer로 받아온 Property값을 ITemIDs로 받아옴
// =====>>>>>>캐릭터 Mesh변경 해주는 함수
void APawn_Player::SetCostumeArray(const TArray<FString>& ITemIDs)
{
	//Instance에 있는 FindITem 함수 이용하기
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

// UserTitleData = InstanceID  =>> ItemID배열로 담아주는 함수
TArray<FString> APawn_Player::UploadPlayer()
{
	TArray<FString> ItemIDs;
	if (BP_ActorComponent_Playfab)
	{
		TArray<FString> TitleKeys;
		TArray<FString> TitleValue;

		// TitleKeys에 모든 키 채워서
		BP_ActorComponent_Playfab->UserTitleData.GetKeys(TitleKeys);

		// TitleValue 에 값 저장
		for (auto keys : TitleKeys)
		{
			TitleValue.Push(*BP_ActorComponent_Playfab->UserTitleData.Find(keys));
			
		}
		// 일단 모든 value 뽑아놓고
		// 데이터 비교
		for (auto propertys : BP_ActorComponent_Playfab->getInventoryItemList())
		{
			for (auto it : TitleValue)
			{
				if (it == propertys.ItemInstanceId)
				{
					// 같은 instanceID인지 체크 후 배열로 저장
					//UE_LOG(LogTemp, Log, TEXT("// instanceID : %s , ItemID : %s "), *it, *propertys.ItemId);
					ItemIDs.Push(propertys.ItemId);
				}
			}

		}
	}

	return ItemIDs;
}

// 메쉬 변경 시키는 함수 
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

// 처음시작시 PlayFab데이터가 없을 경우 default로 메쉬 채워주는 함수
void APawn_Player::BeginDefalutMesh()
{

	//Instance에 있는 데이터 테이블 가지고오기
	UGameInstance_Solaseado* MyInstance = Cast<UGameInstance_Solaseado>(GetGameInstance());
	TArray<UDataTable*>InstanceDataTables = MyInstance->GetDataTables();


	if (!InstanceDataTables.IsValidIndex(0)) // 데이터 테이블이 없다면 탈출
	{
		//UE_LOG(LogTemp, Warning, TEXT("// Nodata !!!!"));
		return;
	}
	// 데이터 테이블이 있다면
	for (auto Table : InstanceDataTables)
	{
		//받아온 데이터 테이블 배열에서 각 행이름 String "1" 의 데이터를 받아오기
		FCustomizing_Struct CustomStruct = *Table->FindRow<FCustomizing_Struct>("1", "");
		if (!CustomStruct.ItemID.IsEmpty())
		{
			//데이터가 제대로 담겼는지 확인 후 changeMesh 함수 실행
			ChangeMesh(CustomStruct.ClassName, CustomStruct.Mesh);
		}
	}
	// 데이터 전부 담고나서 비지블켜기
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
	// 줌인 : -Axis , 줌아웃  + Axis; 
	isZoomIn ? zoomValue -= zoomAxis : zoomValue += zoomAxis;
	

	// 적용된 값을 SpringArm에 적용
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

//친구 요청 업데이트
void APawn_Player::UpdateFriendRequests(FString PlayfabID, FString PlayName)
{
	FFriendStruct FriendRequest;
	FriendRequest.PlayFabID = PlayfabID;
	FriendRequest.TitleID = PlayName;

	if (BP_ActorComponent_Playfab->FriendList.Find(PlayName)==nullptr)
	{
		BP_ActorComponent_Playfab->FriendRequestList.Add(PlayName, FriendRequest);
		//플레이어 알람 추가 예정 줄 수도 있음
	}
}
