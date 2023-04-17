// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY(BlueprintReadOnly, Category = "Playfab")
		int Test_LoadingCount;

	// ���� ��ȭ 
	UPROPERTY(BlueprintReadWrite, Category = "Playfab")
		FString VirtualCoin;

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
	// �κ��丮 ���� ���� 
	UFUNCTION(BlueprintImplementableEvent)
		// void updateInventoryCoin(const FString& coin);
		void updateInventoryCoin();

	// �ΰ��� ���� �ε� ī��Ʈ ó�� _ Test
	UFUNCTION(BlueprintImplementableEvent)
		void Check_getIngameLoadingCount();

	// ĳ���� ���� �Ϸ� �� ���� ó��
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_CreateCharacter();
	
};
