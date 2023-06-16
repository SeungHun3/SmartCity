// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Struct_Customizing.h"

#include "Widget_ShopSlot.generated.h"

/**
 * 
 */
UCLASS()
class TEST_LIDARSAMPLE_API UWidget_ShopSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FITemInfo Item_Infomation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCustomizing_Struct Item_Property;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Is_Selected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Purchased;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		bool Default_Image();

};
