// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_NameTag.generated.h"


UCLASS()
class TEST_LIDARSAMPLE_API UWidget_NameTag : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_NameTag;

public:
	
	UFUNCTION(BlueprintCallable)
		void setPlayerNametag(const FString& name);

};
