// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent_PlayfabStore.h"
#include "Pawn_Player.h"

#include "Core/PlayFabClientAPI.h"
#include "GameModeBase_Solaseado.h"
#include "Widget/Widget_Popup.h"
#include "PlayFabJsonObject.h"
#include "PlayFabJsonValue.h"
using namespace PlayFab;

// Sets default values for this component's properties
UActorComponent_PlayfabStore::UActorComponent_PlayfabStore()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UActorComponent_PlayfabStore::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void UActorComponent_PlayfabStore::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

// Playfab Store Error 
void UActorComponent_PlayfabStore::StoreErrorScript(const PlayFab::FPlayFabCppError& error)
{
	FString str = error.GenerateErrorReport();
	UE_LOG(LogTemp, Log, TEXT("// Playfab Store Error Script ( %s ):: %s"), *error.ErrorName, *str);
	// 아이템 구매 실패
	// "WrongPrice" || "WrongVirtualCurrency") // ItemNotFound , StoreNotFound
	if (error.ErrorName == "InsufficientFunds")
	{
		// 돈 부족 Error
		UE_LOG(LogTemp, Log, TEXT("// Playfab Store Error _ InsufficientFunds "));
		// 팝업
		
		AGameModeBase_Solaseado* GM_Solaseado = Cast<AGameModeBase_Solaseado>(GetWorld()->GetAuthGameMode());
		GM_Solaseado->CreatePopup();
		//WidgetPopup->CheckPopup(enum_PopupStyle::Popup1Key, FText::FromString("fefefefef"), FText::FromString("fefefef"), enum_PopupRun::Cancel);
	}
}

void UActorComponent_PlayfabStore::getStoreItemList(const FString& CatalogVersion, const FString& StoreID)
{

	if (CatalogVersion.IsEmpty() || StoreID.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("// Empty CatalogVersion or StoreID"));
		return;
	}

	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	ClientModels::FGetStoreItemsRequest request;

	// 상점 정보 셋팅
	request.CatalogVersion = CatalogVersion;
	FString version = request.CatalogVersion;
	request.StoreId = StoreID;
	FString StoreId = request.StoreId;

	ClientAPI->GetStoreItems(
		request,
		UPlayFabClientAPI::FGetStoreItemsDelegate::CreateLambda([&, version, StoreId](const ClientModels::FGetStoreItemsResult& result) {

			TArray<FITemInfo> ShopDatas;

			for (auto it : result.Store)
			{
				if (it.VirtualCurrencyPrices.Contains("GC"))
				{
					//GC코인을 가지고 있는 Store 중에서
					FITemInfo infoTemp;
					infoTemp.ItemID = it.ItemId;
					infoTemp.ItemPrice = *it.VirtualCurrencyPrices.Find("GC");
					infoTemp.StoreID = StoreId;
					infoTemp.VirtualCurrency = "GC";
					infoTemp.CatalogVersion = version;
					ShopDatas.Push(infoTemp);

				}
			}
			// 전부 담고나서 배열을 넘겨주기
			if (PlayerOwner)
			{
				PlayerOwner->UpdateStore(ShopDatas);
			}

			}), 

		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_PlayfabStore::StoreErrorScript)
		);
}

void UActorComponent_PlayfabStore::PurchaseItem(FITemInfo Item)
{
	
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	// 스토어에서 가져온 아이템 ID와 가격
	PlayFab::ClientModels::FPurchaseItemRequest request;
	request.CatalogVersion = Item.CatalogVersion;
	request.ItemId = Item.ItemID;
	request.VirtualCurrency = "GC";
	request.Price = Item.ItemPrice;
	request.StoreId = Item.StoreID;

	ClientAPI->PurchaseItem(
		request,
		PlayFab::UPlayFabClientAPI::FPurchaseItemDelegate::CreateLambda([&](const PlayFab::ClientModels::FPurchaseItemResult& result) {

				for (auto it : result.Items)
				{
					UE_LOG(LogTemp, Log, TEXT("// PurchaseItem id : %s"), *it.ItemId);

					// 구매 아이템 정보. 단일 구매 기준으로 처리
					PurchaseNewItem.ItemId = it.ItemId;
					PurchaseNewItem.ItemClass = it.ItemClass;
					PurchaseNewItem.ItemInstanceId = it.ItemInstanceId;
					PurchaseNewItem.UnitPrice = it.UnitPrice;
					PurchaseNewItem.RemainingUses = it.RemainingUses;
				}
				//물건 다사고 코인정보 업데이트
				UpdateCoin();
			}), 
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_PlayfabStore::StoreErrorScript)
		);
}

void UActorComponent_PlayfabStore::UpdateCoin()
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	PlayFab::ClientModels::FGetUserInventoryRequest request;
	
	ClientAPI->GetUserInventory(
		request,
		PlayFab::UPlayFabClientAPI::FGetUserInventoryDelegate::CreateLambda([&](const PlayFab::ClientModels::FGetUserInventoryResult& result) {

			// 기존 아이템 정보와 새로 들어온 아이템 정보를 비교하여 > 슬롯 추가하기
			// InventoryProperty; // result.Inventory
			//for (auto it : result.Inventory)
			//{
			// 
			//}
			// 
			// 아이템 구매 정보가 1개일 경우 슬롯 처리 방법 .
			if (PlayerOwner) {
				if (!PurchaseNewItem.ItemId.IsEmpty())
				{
					InventoryProperty.Add(PurchaseNewItem);
					PlayerOwner->Blueprint_AddInventoryItem(PurchaseNewItem);
					PurchaseNewItem.Clear();
				}
			}
			// 아이템 구매 후 소유 코인 변동 처리
			const int* money = result.VirtualCurrency.Find("GC");
			if (PlayerOwner && money)
			{
				PlayerOwner->CoinUpdate(money[0]);
				UE_LOG(LogTemp, Log, TEXT("// MyMoney : %d"), money[0]);
			}

			}));
}
