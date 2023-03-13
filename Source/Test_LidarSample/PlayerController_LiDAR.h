// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController_LiDAR.generated.h"

/**
 * 
 */
UCLASS()
class TEST_LIDARSAMPLE_API APlayerController_LiDAR : public APlayerController
{
	GENERATED_BODY()
	
public:
	FVector moveForard;
	FVector moveRight;

	// 속도 m/s
	UPROPERTY(EditAnywhere)
		float AutoMoveSpeed = 120.f;

protected:
	// 인풋 
	UFUNCTION()
		void moveForwardEvent(float axis);
	UFUNCTION()
		void moveRightEvent(float axis);

	// 자동이동
	UFUNCTION()
		void moveLocation();
	
	FVector CurrentLoc;
	FVector TargetLoc;

	bool bAutoMoveChecker;

	// 캐릭터 정보
	class ACharacter_Player* Player;
	// 게임 모드 
	class ATest_LidarSampleGameModeBase* gm;

	// 자동이동 컨트롤 핸들러
	FTimerHandle AutomoveHandle;
public:

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
		void AutoMove(FVector Loc);
	UFUNCTION(BlueprintCallable)
		void StopAutoMove();

	UFUNCTION(BlueprintImplementableEvent)
		void AutoMoveGoalEvent();
	UFUNCTION(BlueprintImplementableEvent)
		void AutoMoveCameraRotation(FRotator rot);
};
