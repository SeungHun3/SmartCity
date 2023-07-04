// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/GameModeBase.h"

#include "Test_LidarSampleGameModeBase.generated.h"

USTRUCT(BlueprintType)
struct FLiDARQuizData : public FTableRowBase
{
	GENERATED_BODY()

public:
	//- 문제
	UPROPERTY(BlueprintReadWrite)
		FString Key;
	//- 답
	UPROPERTY(BlueprintReadWrite)
		FString Value1;
	//- 설명
	UPROPERTY(BlueprintReadWrite)
		FString Value2;
};

UCLASS()
class TEST_LIDARSAMPLE_API ATest_LidarSampleGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	//UPROPERTY(BlueprintReadWrite)
	//	class AActor_Photon* PhotonClient;

protected:
	ATest_LidarSampleGameModeBase();

	UPROPERTY()
		class UDataTable* LiDARQuizData;

public:
	virtual void StartPlay() override;

	FLiDARQuizData* getLiDARQuiz(const FString& row);

	// 현재 애니메이션 상태 변경 이벤트
	void changeAnim(uint8 anim);

	// 안드로이드 위치 정보 활성화
	UFUNCTION(BlueprintCallable)
		void CheckLocationEnabled();
	UFUNCTION(BlueprintImplementableEvent)
		void CheckLocationEnabledEvent();
	//// 스트리밍 레벨 로드 
	//void LoadStreamLevel(const FString& Level);
};
