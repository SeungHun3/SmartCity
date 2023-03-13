// Copyright Epic Games, Inc. All Rights Reserved.


#include "Test_LidarSampleGameModeBase.h"
#include "Actor_Photon.h"
#include "Account/AccountManager.h"
#include "Kismet/GameplayStatics.h"

#define LevelName FString("SimPoly")

ATest_LidarSampleGameModeBase::ATest_LidarSampleGameModeBase()
{
	// DataTable'/Game/Project/DataTable/Data_LiDARQuiz.Data_LiDARQuiz'
	static ConstructorHelpers::FObjectFinder<UDataTable> FindQuizData(TEXT("/Game/Project/DataTable/Data_LiDARQuiz"));
	if (FindQuizData.Succeeded())
	{
		LiDARQuizData = FindQuizData.Object;
	}
}

void ATest_LidarSampleGameModeBase::StartPlay()
{
	Super::StartPlay();

	// firebase test 
	


}

// ��� ���� ���� ������ 
FLiDARQuizData* ATest_LidarSampleGameModeBase::getLiDARQuiz(const FString& row)
{
	return LiDARQuizData->FindRow<FLiDARQuizData>(FName(*row), "");
}

// photon Event 1 == walk , 0 == idle
void ATest_LidarSampleGameModeBase::changeAnim(uint8 anim)
{
	checkf(PhotonClient, TEXT("// GameModeBase :: PhotonClient NULL"));
	if(PhotonClient)
		PhotonClient->setPlayerAnimationData(anim);
}

//// Load Level System
//void ATest_LidarSampleGameModeBase::LoadStreamLevel(const FString& Level)
//{
//	// �ܰ� ���� ��� 
//	FLatentActionInfo latent;
//	latent.CallbackTarget = this;
//	latent.UUID = GetUniqueID();
//	latent.Linkage = 0;
//
//	// �ܰ� ���� ��� Ƚ��; 4 ~ 1 ;
//	// ��� Ƚ�� Ž�� Ƚ���� Ž�� ���� Row+-1 Col+- Ž�� �� ����.
//
//	OutsideSteamCnt--;
//	if (OutsideSteamCnt > 0)
//	{
//		latent.ExecutionFunction = "LoadStreamLevelFinish";
//		UGameplayStatics::LoadStreamLevel(this, FName(Level + LevelName), true, true, latent);
//	}
//	else
//	{
//		// �Ϸ�.
//		UE_LOG(LogTemp, Log, TEXT("// Check Outside Streaming End"));
//	}
//}
