// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMesh.h"

#include "GameFramework/Pawn.h"
#include "Pawn_Basic.generated.h"

UCLASS()
class TEST_LIDARSAMPLE_API APawn_Basic : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawn_Basic();

	// Ä³¸¯ÅÍ ½ºÄÌ·¹Åæ ¼³Á¤
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UCapsuleComponent* Root;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent* Body;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent* Hair;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent* Top;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent* Bottoms;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent* Shoes;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
