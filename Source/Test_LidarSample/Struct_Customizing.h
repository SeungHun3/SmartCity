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

 // ������ ����
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

///�˾�////

UENUM(BlueprintType)
enum class enum_PopupRun : uint8
{
	Exit,
	Login,
	CreateRoom,
	// �κ� , ä�� 
	JoinRoom,
	// �κ� , ä�� 
	LeaveRoom, // inLobby
	Channel,
	// ĳ���� ����
	CreateCharacter,
	// ������ ����
	Purchage,
	// ������ �Ǹ�
	Sell,
	// ����
	Notice,
	// ģ�� �߰� �޼��� ������
	AddFriendMessage,
	// ģ�� �߰�
	AddFriend,
	MoveChannel,
	Cancel,
	// Ű ���� �˾�
	Tutorial,
	StartStoryTutorial,
};

UENUM(BlueprintType)
enum class enum_PopupStyle : uint8
{
	Popup1Key,
	Popup2Key,
};


// ����, ���������̺��
USTRUCT(Atomic, BlueprintType)
struct FChecking_Reward : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Gold;
};

// ����Ʈ �������
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
		int Quest_Name = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Quest_Step = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<bool> indexCheck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UDataTable* QuestTable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int reward;
};
//����

USTRUCT(Atomic, BlueprintType)
struct FAchieve_Info : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Explain;

};