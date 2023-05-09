// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Struct_Customizing.generated.h"
/**
 * 
 */

 // 아이템 정보
USTRUCT(BlueprintType)
struct FItemproperty
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemInstanceId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int RemainingUses;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int UnitPrice;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString colorData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bItemEquipment = false;
};

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString CatalogVersion;

};


class TEST_LIDARSAMPLE_API Struct_Customizing
{
public:
	Struct_Customizing();
	~Struct_Customizing();
};
