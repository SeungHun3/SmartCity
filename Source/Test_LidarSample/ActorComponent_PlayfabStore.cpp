// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent_PlayfabStore.h"
#include "Pawn_Player.h"

#include "Core/PlayFabClientAPI.h"

#include "PlayFabJsonObject.h"
#include "PlayFabJsonValue.h"
using namespace PlayFab;

// Sets default values for this component's properties
UActorComponent_PlayfabStore::UActorComponent_PlayfabStore()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UActorComponent_PlayfabStore::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UE_LOG(LogTemp, Log, TEXT("// Store BeginPlay"));

}

// Called every frame
void UActorComponent_PlayfabStore::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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
	request.CatalogVersion = CatalogVersion; //"SolaseadoStore";
	FString version = request.CatalogVersion;
	request.StoreId = StoreID; //"SeungHunTest";
	FString StoreId = request.StoreId;


	ClientAPI->GetStoreItems(
		request,// 요구사항 담아서 전송
		// 회신
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
			}}), 


			// 에러로그
			PlayFab::FPlayFabErrorDelegate::CreateLambda([&](const PlayFab::FPlayFabCppError errors)
				{
					FString error = errors.ErrorMessage;
					UE_LOG(LogTemp, Log, TEXT("// GetStoreItem Error : %s"), *error);
			
				})
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
					it.ItemId;
					UE_LOG(LogTemp, Log, TEXT("// PurchaseItem id : %s"), *it.ItemId);
				
				}
				//물건 다사고 코인정보 업데이트
				UpdateCoin();


				// 인벤토리도 업데이트해야할지도?
		
			}), PlayFab::FPlayFabErrorDelegate::CreateLambda([&](const PlayFab::FPlayFabCppError errors)
				{

					FString error = errors.ErrorMessage;
					UE_LOG(LogTemp, Log, TEXT("// PurchaseItem Error : %s"), *error);
				}));


					
}

void UActorComponent_PlayfabStore::UpdateCoin()
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	PlayFab::ClientModels::FGetUserInventoryRequest request;
	
	ClientAPI->GetUserInventory(
		request,
		PlayFab::UPlayFabClientAPI::FGetUserInventoryDelegate::CreateLambda([&](const PlayFab::ClientModels::FGetUserInventoryResult& result) {
			const int* money = result.VirtualCurrency.Find("GC");

			if (PlayerOwner && money)
			{
				PlayerOwner->CoinUpdate(money[0]);
				UE_LOG(LogTemp, Log, TEXT("// MyMoney : %d"), money[0]);

			}

			}));
}
