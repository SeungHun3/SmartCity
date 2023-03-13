// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Main.generated.h"

struct FLiDARQuizData;

UCLASS()
class TEST_LIDARSAMPLE_API UWidget_Main : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_Message;	// 36자 제한
	UPROPERTY(meta = (BindWidget))
		class UOverlay* Overlay_WorldMessage;
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* ScrollBox_MessageSlot;

	// 메뉴 버튼 이벤트
	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Camera;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Event_End;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button_Event_Start;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button_AutoMove;
	
	// 인원수
	UPROPERTY(meta = (BindWidget))
		class UOverlay* Overlay_Player;
	//UPROPERTY(meta = (BindWidget))
	//	class UTextBlock* TextBlock_CurrentPlayer_LiDAR;
	//UPROPERTY(meta = (BindWidget))
	//	class UTextBlock* TextBlock_CurrentPlayer_Client;
	//UPROPERTY(meta = (BindWidget))
	//	class UTextBlock* TextBlock_CurrentPlayer_Total;

	// 인원수 바인딩 
	UPROPERTY(BlueprintReadOnly)
		float CurrentPlayer_LiDAR = 0.f;
	UPROPERTY(BlueprintReadOnly)
		float CurrentPlayer_Client = 0.f;
	UPROPERTY(BlueprintReadOnly)
		float CurrentPlayer_Total = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ProgressMaxCnt = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ProgressVelocityMaxCnt = 1.f;

	// 플레이어 데이터 평균 수치 ( 속도 , 높이 ) // 사이클 프로그래스 바 변경
	/*UPROPERTY(meta = (BindWidget))
		class UProgressBar* ProgressBar_Velocity;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* ProgressBar_Height;*/

	// 타이머 
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_Timer;
	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* HorizontalBox_Timer;
	// FPS 
	UPROPERTY(BlueprintReadWrite)
		float GameFPS = 0.f;

	// 타이머 카운트
	UPROPERTY(EditAnywhere)
		int MessageTimer = 20;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UWidget_MessageSlot> SpawnSlot;

protected:
	virtual void NativeConstruct() override;

	// 공지, 채팅 월드 메세지 클리어
	UFUNCTION()
		void CloseMessageBox();
	UFUNCTION()
		void ClearTimer();

	FTimerHandle TextTimer;
	int TempMessageTimer;

	FLiDARQuizData* CurrentQuizData;

	// fps widget Checker 
	uint8 Checker_Fps = 0;
	
public:
	// 메세지 출력
	UFUNCTION(BlueprintCallable)
		void setMessageBox(const FText& message, bool bTimer = false);
	// 메세지 출력 타이머 일시정지
	UFUNCTION(BlueprintCallable)
		void PauseMessageTimer(bool bPause);

	// 플레이어 데이터 평균 수치 ( 속도 , 높이 ) 바인딩 함수
	UFUNCTION(BlueprintImplementableEvent)
		void setCircleProgress_Height(float height);
	UFUNCTION(BlueprintImplementableEvent)
		void setCircleProgress_VelocityX(float x);
	UFUNCTION(BlueprintImplementableEvent)
		void setCircleProgress_VelocityY(float y);

	// (LiDAR ) Quiz Event 
	UFUNCTION(BlueprintImplementableEvent)
		void PhotonEndQuizEvent();
	UFUNCTION(BlueprintImplementableEvent)
		void PhotonPauseQuizEvent(bool ev);
	UFUNCTION(BlueprintImplementableEvent)
		void PhotonSelectQuizEvent(const FString& k, const FString& v1, const FString& v2);

	// 채팅 리스트 추가 
	void AddMessageSlot(const FString& id, const FString& Message);

	// 채팅 리스트 클리어
	UFUNCTION(BlueprintCallable)
		void ClearMessageSlot();

	// fps Checker widget Func
	UFUNCTION(BlueprintImplementableEvent)
		void ChangeFpsWidget(uint8 Fps);

	// 타이머 메세지 출력 종료 알림
	UFUNCTION(BlueprintImplementableEvent)
		void OnClearTimerEvent();

	// 타이머 메세지 출력
	void setMessageTimer();

	// 현재 인원
	void setCurrentPlayer(uint8 lidar = 0, uint8 client = 0);

	// ( LiDAR ) 플레이어 정보 평균 값 
	void setLiDARPlayerInfo(float vel, float height);
	void setLiDARVelocity(float x, float y);

	// 게임 프레임 표기
	void setFPS(int fps);

	// ( LiDAR ) 게임 퀴즈 시작 안내 메세지
	void startLiDARQuizEvent(int timer);

	// ( LiDAR ) 게임 o,x 퀴즈 출력 // 문제 출력 후 타이머 출력
	void getLiDARQuizEvent(const FString& number);

	// ( LiDAR ) 게임 퀴즈 종료 안내 메세지
	void EndLiDARQuizEvent();

};
