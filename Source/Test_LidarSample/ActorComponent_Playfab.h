// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlayFab.h"
#include "Core/PlayFabError.h"
#include "Core/PlayFabClientDataModels.h"
#include "Engine/DataTable.h"


#include "Components/ActorComponent.h"
#include "ActorComponent_Playfab.generated.h"

// 금칙어 정보 
USTRUCT(BlueprintType)
struct FBadNameTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString BadString;
};

// 아이템 정보
USTRUCT(BlueprintType)
struct FItemproperty
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemInstanceId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int RemainingUses;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int UnitPrice;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString colorData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bItemEquipment = false;
};


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int RemainingUses;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int UnitPrice;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString colorData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bItemEquipment = false;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_LIDARSAMPLE_API UActorComponent_Playfab : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActorComponent_Playfab();

	// PlayFab Property
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString PlayFabID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString LastLoginTime;

	// Playfab DisplayName
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString UserCharacterName;
	// 유저 플레이 정보 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<FString, FString > UserTitleData;

	// 캐릭터 생성 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCheckerCreateChracter = false;
	// 닉네임 금칙어 확인
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bForbidden = false;

	// 캐릭터 생성 여부에 따른 이벤트 처리
	//UFUNCTION(BlueprintImplementableEvent)
	//	void InitPlayFabUserTitleData(bool Checker);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Playfab 스크립트 호출 실패
	void ErrorScript(const PlayFab::FPlayFabCppError& error);
	// Playfab 스크립트 호출 콜벡
	void SuccessScriptEvent(const PlayFab::ClientModels::FExecuteCloudScriptResult& result);

	FString ErrorFunctionName;

	// Player Playfab 데이터 연동을 위한 바인딩
	class APawn_Player* PlayerOwner;

	// 인벤토리 아이템 정보 리스트
	TArray<FItemproperty> InventoryProperty;

	// 금칙어 데이터 테이블
	UDataTable* BadNameTable;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 금칙어 체크 
	bool FindCheckBadName(const FString& name);

	// Playfab 간편 로그인
	UFUNCTION(BlueprintCallable)
		void Login_Custom(const FString& customid);
	UFUNCTION(BlueprintCallable)
		void Login_Email(const FString& email, const FString& pw);
	// 유저 닉네임 업데이트 (중복 체크, 변경)
	UFUNCTION(BlueprintCallable)
		void updateUserTitleName(const FString& DisplayName);
	// 인게임 로직 전 유저 데이터 정보 로드
	UFUNCTION(BlueprintCallable)
		void getIngamePlayerData();
	// 인벤토리 아이템 정보 데이터
	UFUNCTION(BlueprintCallable)
		const TArray<FItemproperty> getInventoryItemList() { return InventoryProperty; }

	//////////////////////////////////////////////////////////////////
	//// Playfab 스크립트 호출
	//// 스크립트 매개변수 x
	UFUNCTION(BlueprintCallable)
		void ScriptCustomNotField(const FString& FunctionName);
	//// 스크립트 매개변수 o
	UFUNCTION(BlueprintCallable)
		void ScriptCustom(const FString& FunctionName, const FString& FieldName, const FString& value);
	//// 스크립트 매개변수 배열
	UFUNCTION(BlueprintCallable)
		void ScriptCustomArray(const FString& FunctionName, const FString& FieldName, const TArray<FString>& StringArray);


	////////////////////////////////////////////////////////////
	// 승훈 코스튬
	////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable)
	void UploadMyCustom(const FString& FunctionName, const FString& FieldName, const TArray<int> ItemIDs);

	/// 
	///  상점 목록 업데이트
	/// 
	UFUNCTION(BlueprintCallable)
	void getStoreItemList(const FString& CatalogVersion,const FString& StoreID);




	////////////////////////////////////////////////////////////

	// Playfab 스크립트 호출 콜벡 이벤트 처리
	void ScriptResponseEvent(FJsonValue* value);

	//////////////////////////////////////////////////////////// 
	// Return Callback Event 
	// 로그인 성공
	void SuccessLogin(bool NewlyCreated);
	// 유저 타이틀 데이터 가져오기
	void getUserTitleData(FString targetTitle = FString(""));
	// 유저 닉네임 
	void getUserTitleName();
	// 유저 인벤토리 정보
	void getInventoryList();
	// 업적 데이터
	void getStatisticsEvent();
	// 공지 내용
	void getNoticeEvent(int NoticeCount);
	
	

};
