// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Struct_Customizing.generated.h"
/**
 * 
 */

USTRUCT(Atomic, BlueprintType)
struct FITemInfo
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int ItemPrice;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString StoreID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString VirtualCurrency;


};
class TEST_LIDARSAMPLE_API Struct_Customizing
{
public:
	Struct_Customizing();
	~Struct_Customizing();
};
