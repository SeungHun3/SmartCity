// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_CheckingAttandance_Slot.generated.h"

/**
 * 
 */
UCLASS()
class TEST_LIDARSAMPLE_API UWidget_CheckingAttandance_Slot : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
		class UButton* Slot_BTN;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		int  Slot_Coin;



	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Set_Slot(int SlotCount,bool IsGet = false); // 아직 어떤보상인지 정해지지 않음 // 일단 코인으로 함
	virtual bool Set_Slot_Implementation(int SlotCount, bool IsGet = false);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Today_Slot();
	virtual bool Today_Slot_Implementation();
};
