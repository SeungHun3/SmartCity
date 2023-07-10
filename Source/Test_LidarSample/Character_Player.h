// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character_Player.generated.h"

UCLASS()
class TEST_LIDARSAMPLE_API ACharacter_Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacter_Player();

	UPROPERTY(BlueprintReadWrite)
		bool bWalk = false;
	UPROPERTY(BlueprintReadWrite)
		bool isInRoom = false;
	UPROPERTY(BlueprintReadWrite, Category = "Photon")
		int PlayerNr;
	UPROPERTY(BlueprintReadWrite, Category = "Photon")
		FString PlayerName;

	// 채팅 메세지 출력 말풍선
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UWidgetComponent* MessageBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UWidget_Main> MainWdigetClass;

	UPROPERTY(BlueprintReadOnly)
		class UWidget_Main* MainWidget; 

	bool bLocal = false;

	// 참가자 인원수 
	uint8 ParticipantLiDAR;
	uint8 ParticipantClient;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void CloseMessageBox();

	float fDalay = 0.f;

	// 캐릭터 움직임 애니메이션
	uint8 CheckMove;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent)
		void setPlayerNameTag(const FString& name);

	// 위젯 추가
	void AddMainWidget();

	// 메세지 출력 
	void OpenMessageBox(const FString& message);					// 말풍선
	void OpenWorldMessageBox(const FString& message, bool bTimer);	// 스크린

	// 채팅 리스트 추가
	void AddChatList(const FString& id, const FString& Message);

	// 참가중인 인원수 체크
	void AddLiDARPlayerCount();
	void AddClentPlayerCount();

	void RemoveLiDARPlayerCount();
	void RemoveClentPlayerCount();

	// 라이다 데이터 처리
	void setLiDARinformation(float vel, float height);
	void setLiDARVelocity(float X, float Y);

	// 
	void ChangeRoomEvent(uint8 ev);
	//void ChangeEventPause(bool ev);
};
