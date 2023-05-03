// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent_PlayfabStore.h"
#include "Pawn_Player.h"

#include "Core/PlayFabClientAPI.h"

#include "PlayFabJsonObject.h"
#include "PlayFabJsonValue.h"

#include "Struct_Customizing.h"

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
	
}

// Called every frame
void UActorComponent_PlayfabStore::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UActorComponent_PlayfabStore::getStoreItemList(const FString& CatalogVersion, const FString& StoreID)
{

	if (CatalogVersion.IsEmpty() && StoreID.IsEmpty())
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
					//정보 담자마자 업데이트 함수하거나
					ShopDatas.Push(infoTemp);

					//getMyInventory(infoTemp);
				}
			}
			// 전부 담고나서 배열을 넘겨주기
			if (PlayerOwner)
			{
				UE_LOG(LogTemp , Log , TEXT("// Check Store Binding Event :: "));
				// PlayerOwner->UpdateStore(ShopDatas);
			}
			// PlayerOwner->Seunghun_ShopUpdate(ShopDatas);

			}// 에러함수 (넣지않음)
	));


}
