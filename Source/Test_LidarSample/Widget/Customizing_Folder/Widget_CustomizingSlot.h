// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Struct_Customizing.h"
#include "Widget_CustomizingSlot.generated.h"

/**
 * 
 */
UCLASS()
class TEST_LIDARSAMPLE_API UWidget_CustomizingSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget), BlueprintReadWrite)
		class UButton* Slot_BTN;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int SlotCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UDataTable*> DataTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCustomizing_Struct Customizing_Struct;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APawn_Player* CostumePawn;
	
	class UWidget_CustomizingTab* Widget_CustomizingTab;
	class APlayerState_Solaseado* PS;
	// 현재 슬롯 넘버를 가져오는 함수 
	UFUNCTION(BlueprintCallable, BlueprintPure)
		int GetSlotNumber();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void Init_Slot(class APawn_Player* MyCostumePawn ,const TArray<class UDataTable*>& MyDataTable,class UWidget_CustomizingTab* CustomizingTab);
	virtual void Init_Slot_Implementation(class APawn_Player* MyCostumePawn, const TArray<class UDataTable*>& MyDataTable, class UWidget_CustomizingTab* CustomizingTab);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void Change_Slot(int TabNumber);
	virtual void Change_Slot_Implementation(int TabNumber);


	// BP  Set Image
	UFUNCTION(BlueprintImplementableEvent)
		bool Change_Image(class UTexture2D* DataTable_Image, bool IsSelected);


	// Click->ChangeMesh  // 클릭시 슬롯의 정보를 가져와 캐릭터 메쉬변경
	UFUNCTION(BlueprintCallable)
		void ClickEvent();

};
