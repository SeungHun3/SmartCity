// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeBase_Solaseado.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "PlayerState_Solaseado.h"

void AGameModeBase_Solaseado::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGameModeBase_Solaseado::OpenStreamLevel(enum_Level level)
{
	// 추가 레벨 오픈 // 
	FLevel CheckLevel;
	CheckLevel.level = level;
	FLatentActionInfo latent;
	UGameplayStatics::LoadStreamLevel(this, FName(CheckLevel.getLevelName()), true, true, latent);
	StreamLevelFinish.Broadcast(CheckLevel.getLevelName());
}

void AGameModeBase_Solaseado::CloseStreamLevel(enum_Level level)
{
	FLevel CheckLevel;
	CheckLevel.level = level;
	FLatentActionInfo latent;
	UGameplayStatics::UnloadStreamLevel(this, FName(CheckLevel.getLevelName()), latent, false);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// Load Streaming Level Binding Event
void AGameModeBase_Solaseado::LoadStreamEvent()
{
	FLatentActionInfo latent;
	latent.CallbackTarget = this;
	latent.UUID = GetUniqueID();
	latent.Linkage = 0;
	latent.ExecutionFunction = "LoadStreamLevelFinish";

	UGameplayStatics::LoadStreamLevel(this, FName(OpenLevel), true, true, latent);
}

void AGameModeBase_Solaseado::LoadStreamLevelFinish()
{
	CurrentLevel = OpenLevel;
	// 페이트 화면 전환.
	FadeOut();
	// 레벨 이동에 따른 위치 이동 , 위젯 전환 등 추가 작업 진행.
	StreamLevelFinish.Broadcast(FOpenLevel.getLevelName());
}
// 페이드 인 & 아웃
// 레벨 로드시 화면 컨트롤 
void AGameModeBase_Solaseado::StartFadeOut(float From, float To, float Duration)
{
	bool TempFade;
	From >= 1.f ? TempFade = true : TempFade = false;
	if (CheckFade != TempFade)
	{
		CheckFade = TempFade;
		UGameplayStatics::GetPlayerCameraManager(this, 0)->StartCameraFade(From, To, Duration, FLinearColor(0.f, 0.f, 0.f, 1.f), false, true);
	}
}
// 페이드 인
void AGameModeBase_Solaseado::FadeIn()
{
	if(FadeOn)
		StartFadeOut(0.f, 1.f, FadeDuration);
}
// 페이드 아웃
void AGameModeBase_Solaseado::FadeOut()
{
	if (FadeOn)
		StartFadeOut(1.f, 0.f, FadeDuration);
}
void AGameModeBase_Solaseado::CreatePopup()
{
	WidgetCreate_Popup();
	InsufficientFunds_Popup();
}
//////////////////////////////////////////////////////////////////////////////////////////
// 레벨 이동 함수
// BlueprintCallable Func
void AGameModeBase_Solaseado::MoveLevel(enum_Level level , bool bFade)
{
	FOpenLevel.level = level;
	OpenLevel = FOpenLevel.getLevelName();
	// Fade in&out 사용 
	FadeOn = bFade;
	// 화변 전환. Duration time
	FadeIn();

	GetWorld()->GetTimerManager().SetTimer(FadeTimer, this, &AGameModeBase_Solaseado::LoadLevel, 0.01f, false, FadeDuration);
}
// 빠른 레벨 이동 
void AGameModeBase_Solaseado::FastMoveLevel(enum_Level level)
{
	FOpenLevel.level = level;
	OpenLevel = FOpenLevel.getLevelName();
	ChangeLevel(OpenLevel);
}
// 
void AGameModeBase_Solaseado::LoadLevel()
{
	ChangeLevel(OpenLevel);
}

void AGameModeBase_Solaseado::ChangeLevel(const FString& openLevel , bool bFade)
{
	FLatentActionInfo latent;
	latent.CallbackTarget = this;
	latent.UUID = GetUniqueID();
	latent.Linkage = 0;
	OpenLevel = openLevel;

	if (OpenLevel.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("// ChangeLevel String Empty "));
		return;
	}

	// 현재 오픈된 레벨이 존재 하는지 확인하기.
	// 로드 레벨 언로드 후 목표 레벨 로드 하기
	if (CurrentLevel != "")
	{
		latent.ExecutionFunction = "LoadStreamEvent";
		UE_LOG(LogTemp, Log, TEXT("// UnloadStreamLevel :: %s "), *CurrentLevel);
		UGameplayStatics::UnloadStreamLevel(this, FName(CurrentLevel), latent, false);
	}
	else
	{
		latent.ExecutionFunction = "LoadStreamLevelFinish";
		UGameplayStatics::LoadStreamLevel(this, FName(OpenLevel), true, true, latent);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////
// Seamless Load Level System
// 현재 레벨에서 로드할 이웃 레벨 체크
TArray<FString> AGameModeBase_Solaseado::Check_OutsideLevel(const FString& targetLevel)
{
	TArray<FString> OutSideLevel;
	OutSideLevel.Empty();

	// 1. 이름 분리 SimPoly 이후 넘버만 사용
	FString str = "SimPoly";
	FString LevelName = targetLevel;
	if (LevelName.IsEmpty())
		return OutSideLevel;

	const TCHAR* str1 = FCString::Strstr(*LevelName, *str);
	FString Level_Num = FString(str1 + str.Len());
	
	// 레벨 네임 1~16 까지. 환산식에선 배열구조 0 ~ 15 사용 . 추가 변환값 +1 원상 복구.
	int LevelRow = getTargetLevelRow(Level_Num);
	int LevelCol = getTargetLevelCol(Level_Num);

	// 추가 기능  > 8면 추가
	bool bUp = false;
	bool bDown = false;
	bool bRight = false;
	bool bLift = false;

	if (LevelRow + 1 < MaxRow)
	{								
		// +1 추가 변환값.
		OutSideLevel.Add(str + FString::FromInt((LevelRow + 1) * MaxCol + LevelCol + 1));
		bUp = true;
	}
	if (LevelRow - 1 >= 0)
	{
		OutSideLevel.Add(str + FString::FromInt((LevelRow - 1) * MaxCol + LevelCol + 1));
		bDown = true;
	}
	// MaxCol;
	if (LevelCol + 1 < MaxCol)
	{
		OutSideLevel.Add(str + FString::FromInt((LevelRow)*MaxCol + LevelCol + 1 + 1));
		bRight = true;
	}
	if (LevelCol - 1 >= 0)
	{
		OutSideLevel.Add(str + FString::FromInt((LevelRow)*MaxCol + LevelCol - 1 + 1));
		bLift = true;
	}

	//// 8면 추가
	//if (bUp)
	//{
	//	if (bRight)
	//	{
	//		OutSideLevel.Add(str + FString::FromInt((LevelRow + 1) * MaxCol + (LevelCol + 1) + 1));
	//	}
	//	if (bLift)
	//	{
	//		OutSideLevel.Add(str + FString::FromInt((LevelRow + 1) * MaxCol + (LevelCol - 1) + 1));
	//	}
	//}
	//if (bDown)
	//{
	//	if (bRight)
	//	{
	//		OutSideLevel.Add(str + FString::FromInt((LevelRow - 1) * MaxCol + (LevelCol + 1) + 1));
	//	}
	//	if (bLift)
	//	{
	//		OutSideLevel.Add(str + FString::FromInt((LevelRow - 1) * MaxCol + (LevelCol - 1) + 1));
	//	}
	//}
	return OutSideLevel;
}

// 주변 이웃 레벨 언로드.
void AGameModeBase_Solaseado::Seamless_UnloadOutsideLevel(const TArray<FString>& unload)
{
	Check_UnloadLevel = unload;
	// OpenLevel 제외
	Check_UnloadLevel.Remove(OpenLevel);
	// Check_UnloadLevel 목록 전체 언로드 작업.
	Check_UnloadOutsideLevel();
}
void AGameModeBase_Solaseado::Seamless_LoadOutsideLevel(const TArray<FString>& unload)
{
	Check_LoadLevel = unload;
	// CurrentLevel 제외
	Check_LoadLevel.Remove(CurrentLevel);
	// Check_UnloadLevel 목록 전체 언로드 작업.
	Check_LoadOutsideLevel();
}

// seamless 레벨 언로드시 OpenLevel은 언로드 금지.
void AGameModeBase_Solaseado::Check_UnloadOutsideLevel()
{
	// 이웃 레벨 언로드 이름
	FString Check_OutsideUnloadLevel;
	if (Check_UnloadLevel.Num() > 0)
	{
		Check_OutsideUnloadLevel = Check_UnloadLevel[0];
	}
	else
	{
		// 이웃 레벨 언로드 완료 후 > 필요 레벨 로드 작업 추가.
		Seamless_LoadOutsideLevel(Check_OutsideLevel(OpenLevel));
		return;
	}
	
	FLatentActionInfo latent;
	latent.CallbackTarget = this;
	latent.UUID = GetUniqueID();
	latent.Linkage = 0;

	latent.ExecutionFunction = "Check_UnloadOutsideLevel";
	UGameplayStatics::UnloadStreamLevel(this, FName(Check_OutsideUnloadLevel), latent, false);

	Check_UnloadLevel.Remove(Check_OutsideUnloadLevel);
}
// seamless 레벨 언로드시 OpenLevel은 언로드 금지.
void AGameModeBase_Solaseado::Check_LoadOutsideLevel()
{
	// 이웃 레벨 언로드 이름
	FString Check_OutsideloadLevel;
	if (Check_LoadLevel.Num() > 0)
	{
		if(Check_LoadLevel.IsValidIndex(0))
			Check_OutsideloadLevel = Check_LoadLevel[0];
	}
	else
	{
		// Finished >> 필요 레벨 로드 완료 후 처리.
		CurrentLevel = OpenLevel;
		UE_LOG(LogTemp, Log, TEXT("// Test _ Seamless OutsideLevel Check_LoadLevel"));
		return;
	}

	FLatentActionInfo latent;
	latent.CallbackTarget = this;
	latent.UUID = GetUniqueID();
	latent.Linkage = 0;

	latent.ExecutionFunction = "Check_LoadOutsideLevel";
	UGameplayStatics::LoadStreamLevel(this, FName(Check_OutsideloadLevel), true, true, latent);

	Check_LoadLevel.Remove(Check_OutsideloadLevel);
}
//  Seamless Level Load 
void AGameModeBase_Solaseado::SeamlessLevelLoad(const FString& openLevel)
{
	// 다음 오픈 레벨 체크.
	OpenLevel = openLevel;
	// 언로드 로직 완료 후 주변 레벨 로드 작업.
	// 현재 레벨 (CurrentLevel) 해당하는 이웃 레벨 처리.
	UE_LOG(LogTemp, Log, TEXT("// SeamlessLevelLoad CurrentLevel( %s ) "), *CurrentLevel);
	Seamless_UnloadOutsideLevel(Check_OutsideLevel(CurrentLevel));
}

APlayerState_Solaseado* AGameModeBase_Solaseado::get_PlayerState_Solaseado()
{
	return 	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPlayerState<APlayerState_Solaseado>();
}
