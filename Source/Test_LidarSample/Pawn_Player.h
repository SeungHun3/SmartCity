// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Struct_Customizing.h"
#include "GameFramework/Pawn.h"
#include "Pawn_Player.generated.h"

UCLASS()
class TEST_LIDARSAMPLE_API APawn_Player : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawn_Player();

	UPROPERTY(BlueprintReadWrite)
		bool bWalk = false;
	UPROPERTY(BlueprintReadWrite, Category = "Photon")
		int PlayerNr;
	UPROPERTY(BlueprintReadWrite, Category = "Photon")
		FString PlayerName;
	UPROPERTY(BlueprintReadWrite)
		bool isInRoom = false;

	UPROPERTY(BlueprintReadOnly, Category = "Playfab")
		int Test_LoadingCount;

	// 가상 통화 
	UPROPERTY(BlueprintReadWrite, Category = "Playfab")
		FString VirtualCoin;
	UPROPERTY(BlueprintReadWrite, Category = "Playfab")
		class UActorComponent_PlayfabStore* BP_ActorComponent_Playfab;
	//로컬 플레이어인지
	bool bLocal = false;

	//참가 인원수
	uint8 ParticipantClient;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		FString getLoginDeviceID();
	////////////////////////////////////////////////////////
	//포톤

	//포톤에서 받은 코스튬 데이터를 입력받는다.
	UFUNCTION(BlueprintCallable)
	void SetCostumeArray(const TArray<FString>& costumeArray);

	/////////////////////////////////////////////////////////////////////////////////
	// Player Playfab Data Binding 
	
	// 캐릭터 생성 여부에 따른 로직 변경점
	UFUNCTION(BlueprintImplementableEvent)
		void InitPlayFabUserTitleData(bool bChecker);
	// 캐릭터 생성 닉네임 확인
	UFUNCTION(BlueprintImplementableEvent)
		void CustomizingWidgetNameChecker(bool bChecker);
	// 캐릭터 생성 닉네임 화인 + 금칙어 
	UFUNCTION(BlueprintImplementableEvent)
		void updateDisplayNameEvent(bool bChecker);
	// 인벤토리 & 코인 정보 업데이트 
	UFUNCTION(BlueprintImplementableEvent)
		// void updateInventoryCoin(const FString& coin);
		// void updateInventoryCoin();
		void updateInventory();

	// 인게임 접속 로딩 카운트 처리 _ Test
	UFUNCTION(BlueprintImplementableEvent)
		void Check_getIngameLoadingCount();

	// 캐릭터 생성 완료 후 로직 처리
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_CreateCharacter();
	
	/////////////////////////////////////////////
	///////상점, 아이템 정보 업데이트
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateStore(const TArray<FITemInfo>& ShopDatas);
	UFUNCTION(BlueprintImplementableEvent)
		void CoinUpdate(int coin);


	//포톤에 플레이팹 데이터 동기화
	UFUNCTION(BlueprintCallable)
		TArray<FString> UploadPlayer();

	// 인벤토리 아이템 장비하기
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_UpdateEquipmentItem(const FString& ItemID);
	// 아이템 구매 후 인벤토리 업데이트.
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_AddInventoryItem(FItemproperty item);
	
	/////////////////////////////////////////////
	// 참가중인 인원수 체크
	void AddClentPlayerCount();
	void RemoveClentPlayerCount();



};
