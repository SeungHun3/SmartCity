// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Pawn_Player.generated.h"

UCLASS()
class TEST_LIDARSAMPLE_API APawn_Player : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawn_Player();

	UPROPERTY(BlueprintReadWrite)
		bool bWalk = false;
	UPROPERTY(BlueprintReadWrite , Category = "Photon")
		int PlayerNr;
	UPROPERTY(BlueprintReadWrite, Category = "Photon")
		FString PlayerName;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/////////////////////////////////////////////////////////////////////////////////
	// Player Playfab Data Binding 
	// 캐릭터 생성 여부에 따른 로직 변경점
	UFUNCTION(BlueprintImplementableEvent)
		void InitPlayFabUserTitleData(bool bChecker);
	// 캐릭터 생성 닉네임 확인
	UFUNCTION(BlueprintImplementableEvent)
		void CustomizingWidgetNameChecker(bool bChecker);
	// 캐릭터 생성 닉네임 화인 + 금칙어 
	UFUNCTION(BlueprintImplementableEvent)
		void updateDisplayNameEvent(bool bChecker);

};
