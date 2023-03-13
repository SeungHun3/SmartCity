// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_SpeechBubble.generated.h"

UCLASS()
class TEST_LIDARSAMPLE_API UWidget_SpeechBubble : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_Message;

	UFUNCTION(BlueprintCallable)
		void setSpeechBox(const FText& message);

};
