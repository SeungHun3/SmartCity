// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlayFab.h"
#include "Core/PlayFabError.h"
#include "Core/PlayFabClientDataModels.h"
#include "Engine/DataTable.h"
#include "Struct_Customizing.h"

#include "Components/ActorComponent.h"
#include "ActorComponent_Playfab.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateFriend);


// 금칙어 정보 
USTRUCT(BlueprintType)
struct FBadNameTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString BadString;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FString > UserTitleData;

	// 캐릭터 생성 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCheckerCreateChracter = false;
	// 닉네임 금칙어 확인
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bForbidden = false;

	////////////////////////////////////////////////////////////
	// 업적데이터= { 로그인, 출석, 미션}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, int>  PlayFab_Statistics;

	// 퀘스트 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FQuest_Info> MyQuest_Info;	// 진행 중인 퀘스트만 담기

	//업적
	FString AchieveName = "achieve_";


	////////////////////////////////
	//친구
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FFriendStruct> FriendList;	//현재 친구 목록

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FFriendStruct> FriendRequestList;	//친구 신청 목록

	UPROPERTY(BlueprintAssignable)
	FUpdateFriend UpdateFriend;
	//////////////////////////////////////////

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
	// 코스튬
	// 저장된ITemIDs의 배열을 PlayFab 에 FunctionName 스크립트로 전달
	UFUNCTION(BlueprintCallable)
	void UploadMyCustom(const FString& FunctionName, const FString& FieldName, const TArray<int> ItemIDs);




	/////////////////////////////////////////////////////////////
	//퀘스트
	
	// 퀘스트 구조체배열에서 조건에 맞는 인덱스 반환
	int FindQuestInfo_Index(const FString& QuestName);
	// PlayFab_Statistics 탐색 -> 퀘스트 있으면 Title데이터 탐색 후 구조체 Add
	void CheckQuestInfo();
	// Json데이터 그대로 가져와 구조체만들기
	FQuest_Info MakeQuestInfo(const FString& QuestName, const FString& JsonParseData);
	// 퀘스트 테이블 가져오기
	UDataTable* FindQuestTable(const FString& QuestName);
	// 퀘스트 Playfab TitleData업데이트
	void Quest_Update_Title(const FString& QuestName);
	// 퀘스트 Playfab Statistic업데이트
	void Quest_Update_Statistic(const FString& QuestName, enum_Quest_Update Update);

	FQuest_Info SetQuestInfo(const FString& QuestName, int Step);
	//같은 진행도를 가진 테이블 행 숫자뽑아서 배열가져오기 // 프로퍼티 Quest_Step 을 진행도로 탐색 (FQuest_Info Quest; Quest.Quest_Step;)
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<int> GetQuestRowNames(const FString& QuestStepProp, class UDataTable* QuestTable);

	int QuestTotalStepcount(const FString& QuestName);


	UFUNCTION(BlueprintCallable)
	void Quest_Start(const FString& QuestName);
	UFUNCTION(BlueprintCallable)
	bool Quest_Finish(const FString& QuestName, int index);
	UFUNCTION(BlueprintCallable)
	void Quest_Next(const FString& QuestName);
	UFUNCTION(BlueprintCallable)
	void Quest_Drop(const FString& QuestName);

	void Quest_Complete(const FString& QuestName);

	bool Quest_Check_isDoing(const FString& QuestName);
	bool Quest_Check_IsComplete(const FString& QuestName);
	bool Quest_Check_IsLastFinished(const FString& QuestName);

	UFUNCTION(BlueprintCallable)
	enum_Quest_Condition Quest_Check_Condition(const FString& QuestName);

	////////////////////////////////////////////////////////////
	// 업적, 플레이팹 리더보드 Name, Value(int) <-- 로그인시 업데이트된 Statistics 데이터
	UFUNCTION(BlueprintCallable)
		void UpdateAchievement(int AchieveNumber);
	UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<FString> GetAchievement();
	//디버그__업적 초기화
	UFUNCTION(BlueprintCallable)
		void Debug_ClearAchievement();


	// 출석데이터 가져오기
	UFUNCTION(BlueprintCallable)
	bool Is_Today_Checked();
	// 출석 카운팅 가져오기
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int Get_Checking_Count();
	// 플레이팹-> 오늘 보상받고 업데이트
	UFUNCTION(BlueprintCallable)
	void Update_Check_Change(class UWidget_CheckingAttandance_Main* Widget,int coin);
	//20일 완료보상
	UFUNCTION(BlueprintCallable)
	void Daily_20th_Reward(class UWidget_CheckingAttandance_Main* Widget,int Coin);

	//디버그__출석체크 초기화
	UFUNCTION(BlueprintCallable)
	void Debug_Clear_Attandance(class UWidget_CheckingAttandance_Main* Widget);
	UFUNCTION(BlueprintCallable)
	void Debug_Clear_Daily(class UWidget_CheckingAttandance_Main* Widget);
	//디버깅
	
	
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

	///////////////////////
	//친구 관련
	// 
	// 친구 리스트 갱신
	UFUNCTION(BlueprintCallable)
	void getFriendListEvent();

	// 친구 업데이트
	void UpdateFriendList(const PlayFab::ClientModels::FGetFriendsListResult& result);

	// 친구 리스트 갱신
	UFUNCTION(BlueprintCallable)
		void getProfileEvent();

	// 로그인 시간 갱신
	UFUNCTION(BlueprintCallable)
	void UpdateLoginTimeEvent();
	// 로그인 업데이트
	void UpdateLoginTIme(const PlayFab::ClientModels::FUpdateUserDataResult& result);
};
