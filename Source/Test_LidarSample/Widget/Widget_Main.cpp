// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_Main.h"
#include "Widget_MessageSlot.h"

#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Components/ScrollBox.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"

#include "../Test_LidarSampleGameModeBase.h"

void UWidget_Main::NativeConstruct()
{
	Super::NativeConstruct();
	Overlay_WorldMessage->SetVisibility(ESlateVisibility::Collapsed);
	HorizontalBox_Timer->SetVisibility(ESlateVisibility::Collapsed);
	// 
	setCurrentPlayer(0.f, 1.f);
}

// 월드 메세지 (공지 ...)
void UWidget_Main::setMessageBox(const FText& message, bool bTimer)
{
	if (message.ToString() == FString("") || message.ToString() == FString(" "))
	{
		CloseMessageBox();
		return;
	}

	// 타이머 실행 여부
	if (!bTimer)
	{
		HorizontalBox_Timer->SetVisibility(ESlateVisibility::Collapsed);
		GetWorld()->GetTimerManager().ClearTimer(TextTimer);
	}
	else
	{
		HorizontalBox_Timer->SetVisibility(ESlateVisibility::HitTestInvisible);
		setMessageTimer();
	}

	// 36자 제한 ? 
	Overlay_WorldMessage->SetVisibility(ESlateVisibility::HitTestInvisible);
	TextBlock_Message->SetText(message);
}

// setMessageBox 타이머 함수 
void UWidget_Main::CloseMessageBox()
{
	Overlay_WorldMessage->SetVisibility(ESlateVisibility::Collapsed);
	TextBlock_Message->SetText(FText::FromString(""));
}

// 채팅 스크롤 위젯 추가 
void UWidget_Main::AddMessageSlot(const FString& id, const FString& Message)
{
	if (SpawnSlot)
	{
		UWidget_MessageSlot* MessageSlotWidget = CreateWidget<UWidget_MessageSlot>(GetWorld(), SpawnSlot);
		if (MessageSlotWidget)
		{
			//UE_LOG(LogTemp, Log, TEXT("// Check Message Create Widget "));
			MessageSlotWidget->AddMessageSlot(id, Message);
			ScrollBox_MessageSlot->AddChild(MessageSlotWidget);
			ScrollBox_MessageSlot->ScrollToEnd();
		}
	}
}

// 채팅 스크롤 위젯 초기화
void UWidget_Main::ClearMessageSlot()
{
	ScrollBox_MessageSlot->ClearChildren();
}

// 타이머 기능 추가
void UWidget_Main::setMessageTimer()
{
	HorizontalBox_Timer->SetVisibility(ESlateVisibility::HitTestInvisible);
	// 초기값 설정
	TempMessageTimer = MessageTimer;
	TextBlock_Timer->SetText(FText::FromString(*FString::FromInt(MessageTimer)));
	// 타이머 
	GetWorld()->GetTimerManager().SetTimer(TextTimer, this, &UWidget_Main::ClearTimer, 1.f, true);
}
// 타이머 일시 정지 
void UWidget_Main::PauseMessageTimer(bool bPause)
{
	if(bPause)
		GetWorld()->GetTimerManager().PauseTimer(TextTimer);
	else
		GetWorld()->GetTimerManager().UnPauseTimer(TextTimer);
}
// 현재 인원수 체크
void UWidget_Main::setCurrentPlayer(uint8 lidar, uint8 client)
{
	Overlay_Player->SetVisibility(ESlateVisibility::HitTestInvisible);

	// max 10 ~ 20 
	CurrentPlayer_LiDAR = lidar / ProgressMaxCnt;
	CurrentPlayer_Client = client / ProgressMaxCnt;
	CurrentPlayer_Total = CurrentPlayer_LiDAR + CurrentPlayer_Client;

	//TextBlock_CurrentPlayer_LiDAR->SetText(FText::FromString(FString::FromInt(lidar)));
	//TextBlock_CurrentPlayer_Client->SetText(FText::FromString(FString::FromInt(client)));
	//// 총인원 추가
	//TextBlock_CurrentPlayer_Total->SetText(FText::FromString(FString::FromInt(lidar + client)));
}

void UWidget_Main::ClearTimer()
{
	TempMessageTimer--;
	if (TempMessageTimer <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TextTimer);
		HorizontalBox_Timer->SetVisibility(ESlateVisibility::Collapsed);

		FTimerHandle MessageHandle;
		GetWorld()->GetTimerManager().SetTimer(MessageHandle, this, &UWidget_Main::CloseMessageBox, 0.01, false);
		// 타이머 종료 외부 이벤트 // 블루프린트
		OnClearTimerEvent();
		return;
	}

	TextBlock_Timer->SetText(FText::FromString(*FString::FromInt(TempMessageTimer)));
}

// 라이다 플레이어 정보 편균 수치 ( 속도크기, 높이 )
void UWidget_Main::setLiDARPlayerInfo(float vel , float height)
{
	setCircleProgress_Height(height);
}
// 라이다 플레이어 정보 편균 수치 ( 속도 )
void UWidget_Main::setLiDARVelocity(float x, float y)
{
	// 0 ~ 0.4 ~ 1.89 ~ 2.0 max 2.0
	setCircleProgress_VelocityX(x / ProgressVelocityMaxCnt);
	setCircleProgress_VelocityY(y / ProgressVelocityMaxCnt);
}

void UWidget_Main::setFPS(int fps)
{
	// 80 ~ 120 // 10 단위 와이파이 UI
	GameFPS = fps;
	
	if (GameFPS > 110.f && Checker_Fps != 1)
	{
		Checker_Fps = 1;
		ChangeFpsWidget(Checker_Fps);
	}
	else if (GameFPS <= 110.f && GameFPS >= 100.f && Checker_Fps != 2)
	{
		Checker_Fps = 2;
		ChangeFpsWidget(Checker_Fps);
	}
	else if (GameFPS <= 100.f && GameFPS >= 90.f && Checker_Fps != 3)
	{
		Checker_Fps = 3;
		ChangeFpsWidget(Checker_Fps);
	}
	else if (GameFPS <= 90.f && GameFPS >= 80.f && Checker_Fps != 4)
	{
		Checker_Fps = 4;
		ChangeFpsWidget(Checker_Fps);
	}
}

// start Quiz 
void UWidget_Main::startLiDARQuizEvent(int timer)
{
	// 시작 문구 // timer = 문구 딜레이 타임
	MessageTimer = timer;
	setMessageBox(FText::FromString(" 시작 문구 "), true);
}

// Quiz 출력
void UWidget_Main::getLiDARQuizEvent(const FString& number)
{
	if (number == FString("0"))
	{
		// end Event
		PhotonEndQuizEvent();
		return;
	}

	ATest_LidarSampleGameModeBase* gm = Cast<ATest_LidarSampleGameModeBase>(GetWorld()->GetAuthGameMode());
	if (gm)
	{
		// 1 ~ 20 랜덤
		CurrentQuizData = gm->getLiDARQuiz(number);
		if (CurrentQuizData)
		{
			/*FLiDARQuizData asd;
			asd.Key = CurrentQuizData->Key;
			asd.Value1 = CurrentQuizData->Value1;
			asd.Value2 = CurrentQuizData->Value2;*/
			
			PhotonSelectQuizEvent(CurrentQuizData->Key, CurrentQuizData->Value1, CurrentQuizData->Value2);
		}
	}
}

// Quiz Timer 
void UWidget_Main::EndLiDARQuizEvent()
{
	// 종료 문구
	MessageTimer = 30;
	setMessageBox(FText::FromString(" 종료 문구 "), true);
}

