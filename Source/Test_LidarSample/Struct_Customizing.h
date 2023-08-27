// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/SkeletalMesh.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"

#include "Struct_Customizing.generated.h"
/**
 *
 */

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

public:
		void Clear() {
		ItemId.Empty();
		ItemInstanceId.Empty();
		ItemClass.Empty();
		RemainingUses = -1;
		UnitPrice = -1;
	};
};

USTRUCT(Atomic, BlueprintType)
struct FITemInfo
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int ItemPrice;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString StoreID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString VirtualCurrency;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString CatalogVersion;

};

USTRUCT(Atomic, BlueprintType)
struct FCustomizing_Struct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		USkeletalMesh* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString ClassName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTexture2D* Image;
};

class TEST_LIDARSAMPLE_API Struct_Customizing
{
public:
	Struct_Customizing();
	~Struct_Customizing();
};

///팝업////

UENUM(BlueprintType)
enum class enum_PopupRun : uint8
{
	Exit,
	Login,
	CreateRoom,
	// 로비 , 채널 
	JoinRoom,
	// 로비 , 채널 
	LeaveRoom, // inLobby
	Channel,
	// 캐릭터 생성
	CreateCharacter,
	// 아이템 구매
	Purchage,
	// 아이템 판매
	Sell,
	// 공지
	Notice,
	// 친구 추가 메세지 보내기
	AddFriendMessage,
	// 친구 추가
	AddFriend,
	MoveChannel,
	Cancel,
	// 키 차단 팝업
	Tutorial,
	StartStoryTutorial,
};

UENUM(BlueprintType)
enum class enum_PopupStyle : uint8
{
	Popup1Key,
	Popup2Key,
};


// 보상, 데이터테이블용
USTRUCT(Atomic, BlueprintType)
struct FChecking_Reward : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Gold;
};

// 퀘스트 진행사항
USTRUCT(Atomic,BlueprintType)
struct FQuest_List : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UDataTable* QuestData;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
		FString Explain;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int reward;
};

USTRUCT(Atomic, BlueprintType)
struct FQuest_Info : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString Quest_Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Quest_Step = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<bool> IsFinished;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UDataTable* QuestTable = nullptr;
};

UENUM(BlueprintType)
enum class enum_Quest_Condition : uint8
{
	NotStart, // 시작가능
	IsDoing, // 진행중
	LastFinished,// 마지막 보상받기버튼 클릭안했을때
	IsComplete, // 완료

};
UENUM()
enum class enum_Quest_Update : uint8	//업적 업데이트용
{
	Drop, // 퀘스트포기
	Start, // 시작
	Complete, // 완료
};
//업적
USTRUCT(Atomic, BlueprintType)
struct FAchieve_Info : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Explain;

};

//Npc Main
USTRUCT(Atomic, BlueprintType)
struct FDialogueNpc_List : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int NpcName = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UDataTable* NpcDialogueTable;
};
//Npc 대화 내용
USTRUCT(Atomic, BlueprintType)
struct FDialogueNpc_Content :public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int idx = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FText Message;
	//버튼 갯수
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int keyNum = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FText Key1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FText Key2;
	//이벤트가 있는 행인지
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool RowEvent = false;
};
//Npc 대화창
UENUM(BlueprintType)
enum class enum_DialogueStyle : uint8
{
	Dialogue1Key,
	Dialogue2Key,
};


//친구
USTRUCT(BlueprintType)
struct FFriendStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString TitleID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString PlayFabID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDateTime LastLogin;
};