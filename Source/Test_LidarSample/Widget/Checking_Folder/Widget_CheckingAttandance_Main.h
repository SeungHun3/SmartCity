// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_CheckingAttandance_Main.generated.h"

/**
 * 
 */
UCLASS()
class TEST_LIDARSAMPLE_API UWidget_CheckingAttandance_Main : public UUserWidget
{
	GENERATED_BODY()
public:
	UWidget_CheckingAttandance_Main(const FObjectInitializer& ObjectInitializer);
		virtual void NativeConstruct() override;

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class APawn_Player* MyPlayer;

		TSubclassOf<class UWidget_CheckingAttandance_Slot> SlotClass;
		TArray<class UWidget_CheckingAttandance_Slot*> SlotArray;
		int TableLength;

		UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
			class UButton* Get_Reward_BTN;
		UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
			class UUniformGridPanel* Main_gridPanel;


		UFUNCTION(BlueprintCallable)
		void Begin_Bind_Setting();

		UFUNCTION()
		void PressReward();

		UFUNCTION()
		void ChangeSlot();
	
};