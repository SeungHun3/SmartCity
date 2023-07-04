// Copyright Epic Games, Inc. All Rights Reserved.


#include "Test_LidarSampleGameModeBase.h"
//#include "Actor_Photon.h"
#include "Account/AccountManager.h"
#include "Kismet/GameplayStatics.h"

#if PLATFORM_ANDROID
	#include "Android/AndroidJNI.h"
	#include "Android/AndroidApplication.h"
	#include <jni.h>
#endif

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

// 행사 문제 출제 데이터 
FLiDARQuizData* ATest_LidarSampleGameModeBase::getLiDARQuiz(const FString& row)
{
	return LiDARQuizData->FindRow<FLiDARQuizData>(FName(*row), "");
}

// photon Event 1 == walk , 0 == idle
void ATest_LidarSampleGameModeBase::changeAnim(uint8 anim)
{
	// checkf(PhotonClient, TEXT("// GameModeBase :: PhotonClient NULL"));
	//if(PhotonClient)
	//	PhotonClient->setPlayerAnimationData(anim);
}

//// Load Level System
//void ATest_LidarSampleGameModeBase::LoadStreamLevel(const FString& Level)
//{
//	// 외각 지역 출력 
//	FLatentActionInfo latent;
//	latent.CallbackTarget = this;
//	latent.UUID = GetUniqueID();
//	latent.Linkage = 0;
//
//	// 외각 지역 출력 횟수; 4 ~ 1 ;
//	// 출력 횟수 탐색 횟수는 탐색 레벨 Row+-1 Col+- 탐색 후 설정.
//
//	OutsideSteamCnt--;
//	if (OutsideSteamCnt > 0)
//	{
//		latent.ExecutionFunction = "LoadStreamLevelFinish";
//		UGameplayStatics::LoadStreamLevel(this, FName(Level + LevelName), true, true, latent);
//	}
//	else
//	{
//		// 완료.
//		UE_LOG(LogTemp, Log, TEXT("// Check Outside Streaming End"));
//	}
//}

// 안드로이드 위치 정보 활성화 추가 코드
void ATest_LidarSampleGameModeBase::CheckLocationEnabled()
{
#if PLATFORM_ANDROID

    // 안드로이드 애플리케이션 객체 가져오기
    if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
    {
        // 안드로이드 컨텍스트 객체 가져오기
        jobject AndroidContext = FAndroidApplication::GetGameActivityThis();
        
        // 안드로이드 위치 관리자 가져오기
        jclass LocationManagerClass = Env->FindClass("android/location/LocationManager");
        jmethodID GetSystemServiceMethodID = Env->GetMethodID(LocationManagerClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
        jobject LocationManagerObj = Env->CallObjectMethod(AndroidContext, GetSystemServiceMethodID, Env->NewStringUTF("location"));

        // 위치 기능이 켜져 있는지 확인
        jmethodID IsProviderEnabledMethodID = Env->GetMethodID(LocationManagerClass, "isProviderEnabled", "(Ljava/lang/String;)Z");
        jboolean GpsEnabled = Env->CallBooleanMethod(LocationManagerObj, IsProviderEnabledMethodID, Env->NewStringUTF("gps"));
        jboolean NetworkEnabled = Env->CallBooleanMethod(LocationManagerObj, IsProviderEnabledMethodID, Env->NewStringUTF("network"));

        if (!GpsEnabled && !NetworkEnabled)
        {
            // 위치 기능이 꺼져 있으면 켜도록 안내
            FString Message = "Please turn on location services.";
            CheckLocationEnabledEvent();
            // FAndroidMisc::ShowAlertBox(TEXT("Location Services"), FText::FromString(Message), FText::FromString("OK"));
        }
    }
#endif
    
}
