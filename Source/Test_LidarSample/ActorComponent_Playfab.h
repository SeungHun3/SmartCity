// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlayFab.h"
#include "Core/PlayFabError.h"
#include "Core/PlayFabClientDataModels.h"
#include "Engine/DataTable.h"


#include "Components/ActorComponent.h"
#include "ActorComponent_Playfab.generated.h"

// ��Ģ�� ���� 
USTRUCT(BlueprintType)
struct FBadNameTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString BadString;
};

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
	// ���� �÷��� ���� 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TMap<FString, FString > UserTitleData;

	// ĳ���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCheckerCreateChracter = false;
	// �г��� ��Ģ�� Ȯ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bForbidden = false;

	// ĳ���� ���� ���ο� ���� �̺�Ʈ ó��
	//UFUNCTION(BlueprintImplementableEvent)
	//	void InitPlayFabUserTitleData(bool Checker);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Playfab ��ũ��Ʈ ȣ�� ����
	void ErrorScript(const PlayFab::FPlayFabCppError& error);
	// Playfab ��ũ��Ʈ ȣ�� �ݺ�
	void SuccessScriptEvent(const PlayFab::ClientModels::FExecuteCloudScriptResult& result);

	FString ErrorFunctionName;

	// Player Playfab ������ ������ ���� ���ε�
	class APawn_Player* PlayerOwner;

	// �κ��丮 ������ ���� ����Ʈ
	TArray<FItemproperty> InventoryProperty;

	// ��Ģ�� ������ ���̺�
	UDataTable* BadNameTable;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ��Ģ�� üũ 
	bool FindCheckBadName(const FString& name);

	// Playfab ���� �α���
	UFUNCTION(BlueprintCallable)
		void Login_Custom(const FString& customid);
	UFUNCTION(BlueprintCallable)
		void Login_Email(const FString& email, const FString& pw);
	// ���� �г��� ������Ʈ (�ߺ� üũ, ����)
	UFUNCTION(BlueprintCallable)
		void updateUserTitleName(const FString& DisplayName);
	// �ΰ��� ���� �� ���� ������ ���� �ε�
	UFUNCTION(BlueprintCallable)
		void getIngamePlayerData();
	// �κ��丮 ������ ���� ������
	UFUNCTION(BlueprintCallable)
		const TArray<FItemproperty> getInventoryItemList() { return InventoryProperty; }

	//////////////////////////////////////////////////////////////////
	//// Playfab ��ũ��Ʈ ȣ��
	//// ��ũ��Ʈ �Ű����� x
	UFUNCTION(BlueprintCallable)
		void ScriptCustomNotField(const FString& FunctionName);
	//// ��ũ��Ʈ �Ű����� o
	UFUNCTION(BlueprintCallable)
		void ScriptCustom(const FString& FunctionName, const FString& FieldName, const FString& value);
	//// ��ũ��Ʈ �Ű����� �迭
	UFUNCTION(BlueprintCallable)
		void ScriptCustomArray(const FString& FunctionName, const FString& FieldName, const TArray<FString>& StringArray);


	////////////////////////////////////////////////////////////
	// ���� �ڽ�Ƭ
	////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable)
	void UploadMyCustom(const FString& FunctionName, const FString& FieldName, const TArray<int> ItemIDs);

	/// 
	///  ���� ��� ������Ʈ
	/// 
	UFUNCTION(BlueprintCallable)
	void getStoreItemList(const FString& CatalogVersion,const FString& StoreID);




	////////////////////////////////////////////////////////////

	// Playfab ��ũ��Ʈ ȣ�� �ݺ� �̺�Ʈ ó��
	void ScriptResponseEvent(FJsonValue* value);

	//////////////////////////////////////////////////////////// 
	// Return Callback Event 
	// �α��� ����
	void SuccessLogin(bool NewlyCreated);
	// ���� Ÿ��Ʋ ������ ��������
	void getUserTitleData(FString targetTitle = FString(""));
	// ���� �г��� 
	void getUserTitleName();
	// ���� �κ��丮 ����
	void getInventoryList();
	// ���� ������
	void getStatisticsEvent();
	// ���� ����
	void getNoticeEvent(int NoticeCount);
	
	

};
