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

	UPROPERTY(VisibleAnywhere)
		UDataTable* Quest_Table;
	UPROPERTY(VisibleAnywhere)
		UDataTable* Achieve_Table;

	UPROPERTY(VisibleAnywhere)
		UDataTable* DialogueNpc_Table;

public:
	UGameInstance_Solaseado();

	UFUNCTION(BlueprintPure, BlueprintCallable)
		TArray<UDataTable*> GetDataTables() { return CustomDataArray; }

	// ITemID로 Mesh, Image,,을 담는 FCustomizing_Struct 구조체 반환처리하는 함수
	UFUNCTION(BlueprintCallable)
		FCustomizing_Struct Find_ITem(const FString& ITemID);

	UFUNCTION(BlueprintPure, BlueprintCallable)
		UDataTable* GetCheckingRewardTables() { return Checking_Reward_Table; }
	UFUNCTION(BlueprintPure, BlueprintCallable)
		UDataTable* GetSpecialRewardTables() { return Special_Reward_Table; }
	UFUNCTION(BlueprintPure, BlueprintCallable)
		UDataTable* GetQuestTables() { return Quest_Table; }
	UFUNCTION(BlueprintPure, BlueprintCallable)
		UDataTable* GetAchieveTables() { return Achieve_Table; }


	UFUNCTION(BlueprintPure, BlueprintCallable)
		UDataTable* GetDialogueNpcTable() { return DialogueNpc_Table;  }

};
