// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "..\..\Struct_Customizing.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Community_Slot.generated.h"

/**
 * 
 */
UCLASS()
class TEST_LIDARSAMPLE_API UWidget_Community_Slot : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FFriendStruct FriendInfo;
public:
};
