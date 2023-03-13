// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_MessageSlot.generated.h"

/**
 * 
 */
UCLASS()
class TEST_LIDARSAMPLE_API UWidget_MessageSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_ID;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_Message;

public:
	UFUNCTION()
		void AddMessageSlot(const FString& id, const FString& Message);
	
};
