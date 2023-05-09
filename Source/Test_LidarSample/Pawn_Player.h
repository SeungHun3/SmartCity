// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Struct_Customizing.h"
#include "GameFramework/Pawn.h"
#include "Pawn_Player.generated.h"

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
	void SetCostumeArray(const TArray<FString>& costumeArray);

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


	//���濡 �÷����� ������ ����ȭ
	UFUNCTION(BlueprintCallable)
		TArray<FString> UploadPlayer();

	// �κ��丮 ������ ����ϱ�
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_UpdateEquipmentItem(const FString& ItemID);
	// ������ ���� �� �κ��丮 ������Ʈ.
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_AddInventoryItem(FItemproperty item);
	
	/////////////////////////////////////////////
	// �������� �ο��� üũ
	void AddClentPlayerCount();
	void RemoveClentPlayerCount();



};
