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
		case enum_Level::SimPoly1: // SimPoly ����
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

	// ���� �ɸ��� ���� ������ ������ �ִ� Row
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int MaxRow = 4;
	// ���� �ɸ��� ���� ������ ������ �ִ� Col
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int MaxCol = 4;

	// Move Level Finished 
	UPROPERTY(BlueprintAssignable, Category = "SH_")
		FStreamLevelFinish StreamLevelFinish;

private:
	// ���̵� ���� �ð�
	float FadeDuration = 3.5f;

	// ���� �̵��� ���̵� ��� ����,
	bool FadeOn;
	// ���� ���̵� ����
	bool CheckFade = true;
	// ī�޶� ȭ�� ��ȯ , ���̵� �� & �ƿ�
	void StartFadeOut(float From, float To, float Duration);

	void FadeIn();
	void FadeOut();

	// ���̵� ���� Ÿ�̸�
	FTimerHandle FadeTimer;
	// �̿� ���� ��ε� ���
	TArray<FString> Check_UnloadLevel;
	// �̿� ���� ��ε� ���
	TArray<FString> Check_LoadLevel;

	UFUNCTION()
		void Check_UnloadOutsideLevel();
	UFUNCTION()
		void Check_LoadOutsideLevel();

	// ���� ������ ������ Row, Col ��ȯ / Level - 1 ��ȯ�� ����.
	int getTargetLevelRow(const FString& target) {
		return (FCString::Atoi(*target) - 1) / MaxRow;
	}
	int getTargetLevelCol(const FString& target) {
		return (FCString::Atoi(*target) - 1) % MaxCol;
	}
	// ���� �ε� �������� �ֺ� �̿� ���� üũ
	TArray<FString> Check_OutsideLevel(const FString& targetLevel);

	// �̿� ���� �ε� & ��ε�
	void Seamless_UnloadOutsideLevel(const TArray<FString>& unload);
	void Seamless_LoadOutsideLevel(const TArray<FString>& unload);

public:
	void OpenStreamLevel(enum_Level level);
	void CloseStreamLevel(enum_Level level);

	// ��ε� �ʿ伺 üũ > ���� �̵�.
	void ChangeLevel(const FString& openLevel, bool bFade = true);
	// ���̵� ��Ȳ ���� �ε� 
	UFUNCTION()
		void LoadLevel();
	UFUNCTION()
		void LoadStreamEvent();
	UFUNCTION()
		void LoadStreamLevelFinish();
	
	UFUNCTION(BlueprintCallable, Category = "SH_")
		void MoveLevel(enum_Level level, bool bFade = true);
	// fade in & out x , ���� ���� �̵�
	UFUNCTION(BlueprintCallable, Category = "SH_")
		void FastMoveLevel(enum_Level level);

	// 
	UFUNCTION(BlueprintCallable, Category = "SH_")
		void SeamlessLevelLoad(const FString& openLevel);
};