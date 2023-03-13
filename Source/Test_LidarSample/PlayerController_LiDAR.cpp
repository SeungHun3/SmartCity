// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController_LiDAR.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Test_LidarSampleGameModeBase.h"
#include "Character_Player.h"
#include "TimerManager.h"
#include "Camera/PlayerCameraManager.h"

void APlayerController_LiDAR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Player)
	{
		// 캐릭터 인풋
		FVector moveCharacter = moveForard + moveRight;
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Player->GetActorLocation() + moveCharacter.GetSafeNormal() * 100.f);
	}
}

void APlayerController_LiDAR::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<ACharacter_Player>(GetPawn());
	gm = Cast<ATest_LidarSampleGameModeBase>(GetWorld()->GetAuthGameMode());
}

void APlayerController_LiDAR::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAxis(FName("moveRight"), this, &APlayerController_LiDAR::moveRightEvent);
	InputComponent->BindAxis(FName("moveForward"), this, &APlayerController_LiDAR::moveForwardEvent);
}

void APlayerController_LiDAR::moveForwardEvent(float axis)
{
	if (bAutoMoveChecker)
		return;

	float checkAxis = FMath::Abs(axis);
	if (checkAxis > 0.f)
	{
		if (!Player->bWalk)
		{
			if(gm) // 포톤 > 캐릭터 애니메이션 처리 함수 
				gm->changeAnim(1);
			Player->bWalk = true;
		}

		FVector vCameraForward = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetActorForwardVector();
		moveForard = axis * FVector(vCameraForward.X, vCameraForward.Y, 0.f);
	}
	else
	{
		moveForard = FVector::ZeroVector;
		if (Player->bWalk && moveRight == FVector::ZeroVector)
		{
			if (gm)
				gm->changeAnim(0);
			Player->bWalk = false;
		}
	}
}

void APlayerController_LiDAR::moveRightEvent(float axis)
{
	if (bAutoMoveChecker)
		return;

	float checkAxis = FMath::Abs(axis);
	if (checkAxis > 0.f)
	{
		if (!Player->bWalk)
		{
			if (gm)
				gm->changeAnim(1);

			Player->bWalk = true;
		}
		FVector vCameraRight = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetActorRightVector();
		moveRight = axis * FVector(vCameraRight.X, vCameraRight.Y, 0.f);
	}
	else
	{
		moveRight = FVector::ZeroVector;
		if (Player->bWalk && moveForard == FVector::ZeroVector)
		{
			if (gm)
				gm->changeAnim(0);
			Player->bWalk = false;
		}
	}
}

// 목표 지점 자동 이동
void APlayerController_LiDAR::AutoMove(FVector Loc)
{
	bAutoMoveChecker = true;
	// UE_LOG(LogTemp, Log, TEXT("// Auto Move Location :: %s"), *Loc.ToString());
	CurrentLoc = GetPawn()->GetActorLocation();
	TargetLoc = Loc;

	// StopAutoMove();
	if (gm) // 포톤 > 캐릭터 애니메이션 처리 함수 
		gm->changeAnim(1);
	Player->bWalk = true;

	GetWorld()->GetTimerManager().SetTimer(AutomoveHandle, this, &APlayerController_LiDAR::moveLocation, 0.01f, true, 0.2f);
}

void APlayerController_LiDAR::moveLocation()
{
	FVector vDistance = TargetLoc - CurrentLoc;
	FVector vMoveDistance = GetPawn()->GetActorLocation() + vDistance.GetSafeNormal() * 0.01f * AutoMoveSpeed;

	GetPawn()->SetActorLocation(vMoveDistance);
	GetPawn()->SetActorRotation(vDistance.GetSafeNormal().Rotation());

	// 도착 체크 
	FVector vGoalTarget = TargetLoc - GetPawn()->GetActorLocation();

	if (vGoalTarget.Size() <= 50.f)
	{
		// Goal 
		// StopAutoMove();
		GetWorld()->GetTimerManager().ClearTimer(AutomoveHandle);
		AutoMoveGoalEvent();
	}
}

void APlayerController_LiDAR::StopAutoMove()
{
	if (gm) // 포톤 > 캐릭터 애니메이션 처리 함수 
		gm->changeAnim(0);

	Player->bWalk = false;
	bAutoMoveChecker = false;
}
