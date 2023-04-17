// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeBase_Solaseado.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void AGameModeBase_Solaseado::BeginPlay()
{
	Super::BeginPlay();

}

void AGameModeBase_Solaseado::OpenStreamLevel(enum_Level level)
{
	// �߰� ���� ���� // 
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
	// ����Ʈ ȭ�� ��ȯ.
	FadeOut();
	// ���� �̵��� ���� ��ġ �̵� , ���� ��ȯ �� �߰� �۾� ����.
	StreamLevelFinish.Broadcast(FOpenLevel.getLevelName());
}
// ���̵� �� & �ƿ�
// ���� �ε�� ȭ�� ��Ʈ�� 
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
// ���̵� ��
void AGameModeBase_Solaseado::FadeIn()
{
	if(FadeOn)
		StartFadeOut(0.f, 1.f, FadeDuration);
}
// ���̵� �ƿ�
void AGameModeBase_Solaseado::FadeOut()
{
	if (FadeOn)
		StartFadeOut(1.f, 0.f, FadeDuration);
}

//////////////////////////////////////////////////////////////////////////////////////////
// ���� �̵� �Լ�
// BlueprintCallable Func
void AGameModeBase_Solaseado::MoveLevel(enum_Level level , bool bFade)
{
	FOpenLevel.level = level;
	OpenLevel = FOpenLevel.getLevelName();
	// Fade in&out ��� 
	FadeOn = bFade;
	// ȭ�� ��ȯ. Duration time
	FadeIn();
	// Check Delta Time 
	float CurrentDeltaTime;
	const UWorld* world = GetWorld();
	if (world)
	{
		CurrentDeltaTime = world->GetDeltaSeconds();
	}
	GetWorld()->GetTimerManager().SetTimer(FadeTimer, this, &AGameModeBase_Solaseado::LoadLevel, 0.01f, false, FadeDuration);
}
// ���� ���� �̵� 
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

	// ���� ���µ� ������ ���� �ϴ��� Ȯ���ϱ�.
	// �ε� ���� ��ε� �� ��ǥ ���� �ε� �ϱ�
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