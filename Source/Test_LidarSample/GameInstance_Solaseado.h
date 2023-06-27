// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Struct_Customizing.h"
#include "Engine/GameInstance.h"
#include "GameInstance_Solaseado.generated.h"

/**
 * 
 */
UCLASS()
class TEST_LIDARSAMPLE_API UGameInstance_Solaseado : public UGameInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
		TArray<UDataTable*> CustomDataArray;

	UPROPERTY(VisibleAnywhere)
		UDataTable* Checking_Reward_Table;

	UPROPERTY(VisibleAnywhere)
		UDataTable* Special_Reward_Table;

public:
	UGameInstance_Solaseado();

	UFUNCTION(BlueprintPure, BlueprintCallable)
		TArray<UDataTable*> GetDataTables() { return CustomDataArray; }

	// ITemID�� Mesh, Image,,�� ��� FCustomizing_Struct ����ü ��ȯó���ϴ� �Լ�
	UFUNCTION(BlueprintCallable)
		FCustomizing_Struct Find_ITem(const FString& ITemID);

	UFUNCTION(BlueprintPure, BlueprintCallable)
		UDataTable* GetCheckingRewardTables() { return Checking_Reward_Table; }
	UFUNCTION(BlueprintPure, BlueprintCallable)
		UDataTable* GetSpecialRewardTables() { return Special_Reward_Table; }

};
