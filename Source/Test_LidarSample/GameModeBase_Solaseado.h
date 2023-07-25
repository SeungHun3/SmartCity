// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Struct_Customizing.h"
#include "GameModeBase_Solaseado.generated.h"

#define SH_LOG(Value) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Value )

UENUM(BlueprintType)
enum class enum_Level : uint8
{
	Intro,
	Login,
	Solaseado = 2,
	SimPoly1,
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
		case enum_Level::SimPoly1: // SimPoly 생략
			str = "SimPoly1";
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

	// 현재 심리스 레벨 구조가 가지는 최대 Row
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int MaxRow = 4;
	// 현재 심리스 레벨 구조가 가지는 최대 Col
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int MaxCol = 4;

	// Move Level Finished 
	UPROPERTY(BlueprintAssignable, Category = "SH_")
		FStreamLevelFinish StreamLevelFinish;


	// 포톤변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AActor_SolaseadoPhoton* PhotonCloud;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AActor_PhotonChat* PhotonChat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AActor_PhotonVoice* PhotonVoice;

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
	// 이웃 레벨 언로드 목록
	TArray<FString> Check_UnloadLevel;
	// 이웃 레벨 언로드 목록
	TArray<FString> Check_LoadLevel;

	UFUNCTION()
		void Check_UnloadOutsideLevel();
	UFUNCTION()
		void Check_LoadOutsideLevel();

	// 현재 레벨이 가지는 Row, Col 변환 / Level - 1 변환값 적용.
	int getTargetLevelRow(const FString& target) {
		return (FCString::Atoi(*target) - 1) / MaxRow;
	}
	int getTargetLevelCol(const FString& target) {
		return (FCString::Atoi(*target) - 1) % MaxCol;
	}
	// 현재 로드 레벨에서 주변 이웃 레벨 체크
	TArray<FString> Check_OutsideLevel(const FString& targetLevel);

	// 이웃 레벨 로드 & 언로드
	void Seamless_UnloadOutsideLevel(const TArray<FString>& unload);
	void Seamless_LoadOutsideLevel(const TArray<FString>& unload);

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
	
	UFUNCTION(BlueprintCallable, Category = "SH_")
		void MoveLevel(enum_Level level, bool bFade = true);
	// fade in & out x , 빠른 레벨 이동
	UFUNCTION(BlueprintCallable, Category = "SH_")
		void FastMoveLevel(enum_Level level);

	// 
	UFUNCTION(BlueprintCallable, Category = "SH_")
		void SeamlessLevelLoad(const FString& openLevel);
	
	// 팝업 추가 함수
	UFUNCTION(BlueprintImplementableEvent)
		void WidgetCreate_Popup();
	UFUNCTION(BlueprintCallable)
		void CreatePopup();
	UFUNCTION(BlueprintImplementableEvent)
		void InsufficientFunds_Popup();
	
	UPROPERTY(BlueprintReadWrite, Category = "MyActors")
		TArray<AActor*> POIActors;
	///////////////////////////////////////
	// State 관리

	// return = state

	UFUNCTION(BlueprintCallable, BlueprintPure)
	class APlayerState_Solaseado* get_PlayerState_Solaseado();

};
