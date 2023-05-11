// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActorComponent_Playfab.h"
#include "Struct_Customizing.h"
#include "ActorComponent_PlayfabStore.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_LIDARSAMPLE_API UActorComponent_PlayfabStore : public UActorComponent_Playfab
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActorComponent_PlayfabStore();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void StoreErrorScript(const PlayFab::FPlayFabCppError& error);

	// �ű� ���� ������ ���� 
	FItemproperty PurchaseNewItem;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	///  ���� ��� ������Ʈ
	UFUNCTION(BlueprintCallable)
		void getStoreItemList(const FString& CatalogVersion, const FString& StoreID);
	///  ���� ����
	UFUNCTION(BlueprintCallable)
		void PurchaseItem(FITemInfo Item);
	UFUNCTION(BlueprintCallable)
		void UpdateCoin();
		
};