// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeBase_Solaseado.generated.h"

UENUM(BlueprintType)
enum class enum_Level : uint8
{
	Intro,
	Login,
	Solaseado = 2,
	None,
};

struct FLevel
{
public:
	enum_Level level = enum_Level::None;
	FString getLevelName()
	{
		FString str;
		switch (level)
		{
		case enum_Level::Intro:
			str = "Intro";
			break;
		case enum_Level::Login:
			str = "Login";
			break;
		case enum_Level::Solaseado:
			str = "Solaseado";
			break;
		}
		return str;
	}
};

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStreamLevelFinish, enum_Level, level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStreamLevelFinish, const FString& , level);

UCLASS()
class TEST_LIDARSAMPLE_API AGameModeBase_Solaseado : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

	FString CurrentLevel = FString("");
	FString OpenLevel;
	FLevel FOpenLevel;

	// Move Level Finished 
	UPROPERTY(BlueprintAssignable)
		FStreamLevelFinish StreamLevelFinish;

private:
	// 페이드 진행 시간
	float FadeDuration = 3.5f;

	// 레벨 이동시 페이드 사용 여부,
	bool FadeOn;
	// 현재 페이드 상태
	bool CheckFade = true;
	// 카메라 화면 전환 , 페이드 인 & 아웃
	void StartFadeOut(float From, float To, float Duration);

	void FadeIn();
	void FadeOut();

	// 페이드 시작 타이머
	FTimerHandle FadeTimer;

public:
	void OpenStreamLevel(enum_Level level);
	void CloseStreamLevel(enum_Level level);

	// 언로드 필요성 체크 > 레벨 이동.
	void ChangeLevel(const FString& openLevel, bool bFade = true);
	// 페이드 상황 레벨 로드 
	UFUNCTION()
		void LoadLevel();
	UFUNCTION()
		void LoadStreamEvent();
	UFUNCTION()
		void LoadStreamLevelFinish();
	
	UFUNCTION(BlueprintCallable)
		void MoveLevel(enum_Level level, bool bFade = true);
	// 페이트 x , 빠른 레벨 이동
	UFUNCTION(BlueprintCallable)
		void FastMoveLevel(enum_Level level);
};
