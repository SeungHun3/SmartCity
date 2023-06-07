// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerState_Solaseado.generated.h"

/**
 * 
 */
UCLASS()
class TEST_LIDARSAMPLE_API APlayerState_Solaseado : public APlayerState
{
	GENERATED_BODY()


public:


		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidget_Customizing* Widget_Customizing;

		UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UI_Customizing();
};
