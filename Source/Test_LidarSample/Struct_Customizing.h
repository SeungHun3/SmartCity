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
};

UENUM(BlueprintType)
enum class enum_PopupStyle : uint8
{
	Popup1Key,
	Popup2Key,
};