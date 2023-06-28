// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Struct_Customizing.h"

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


		int Checking_Count;
		bool IsTodayChecked;
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class APawn_Player* MyPlayer; 
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
			FChecking_Reward TodayReward;
		
		UPROPERTY(BlueprintReadWrite, EditAnywhere)
			TSubclassOf<class UWidget_CheckingAttandance_Slot> SlotClass;
		TArray<class UWidget_CheckingAttandance_Slot*> SlotArray;
		int TableLength;
		
		UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
			class UButton* Get_Reward_BTN;
		
		UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
			class UUniformGridPanel* Main_gridPanel;
		UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
			class UButton* Button_SpecialReward;



		UFUNCTION(BlueprintCallable)
		void Begin_Bind_Setting();

		UFUNCTION(BlueprintCallable)
		void PressReward();

		UFUNCTION(BlueprintCallable)
		void PressSpecial();

		UFUNCTION()
		void ChangeSlot();

		UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
			void ChangeSpecial();
		virtual void ChangeSpecial_Implementation();


		// 디버깅용
		UFUNCTION(BlueprintCallable)
			void Debug_ClearAttandance();

		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
			void Debug_Finished_Clear();

		UFUNCTION(BlueprintCallable)
			void Debug_ClearCheck();

		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
			void Debug_Finished_DailyClear();
		//여기까지


		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
			FText TodayCount(int Count);
};
