// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActorComponent_Playfab.h"
#include "Struct_Customizing.h"
#include "Widget/Widget_Popup.h"
#include "ActorComponent_PlayfabStore.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_LIDARSAMPLE_API UActorComponent_PlayfabStore : public UActorComponent_Playfab
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActorComponent_PlayfabStore();

	class UWidget_Popup* WidgetPopup;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	void StoreErrorScript(const PlayFab::FPlayFabCppError& error);

	// 신규 구매 아이템 정보 
	FItemproperty PurchaseNewItem;

	// 팝업
	virtual void AddPopupWidget(class UWidget_Popup* target, enum_PopupStyle style, enum_PopupRun run, FText message, FText title) {};

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	///  상점 목록 업데이트
	UFUNCTION(BlueprintCallable)
		void getStoreItemList(const FString& CatalogVersion, const FString& StoreID);
	///  상점 구매
	UFUNCTION(BlueprintCallable)
		void PurchaseItem(FITemInfo Item);
	UFUNCTION(BlueprintCallable)
		void UpdateCoin();

};
