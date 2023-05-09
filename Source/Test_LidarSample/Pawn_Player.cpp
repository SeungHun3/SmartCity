// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_Player.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "ActorComponent_PlayfabStore.h"

// Sets default values
APawn_Player::APawn_Player()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APawn_Player::BeginPlay()
{
	Super::BeginPlay();
	
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


void APawn_Player::SetCostumeArray(const TArray<FString> &costumeArray)
{

}


void APawn_Player::AddClentPlayerCount()
{
	++ParticipantClient;
}

void APawn_Player::RemoveClentPlayerCount()
{
	--ParticipantClient;
}
