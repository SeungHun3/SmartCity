// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent_Playfab.h"
#include "Pawn_Player.h"

#include "Core/PlayFabClientAPI.h"

#include "PlayFabJsonObject.h"
#include "PlayFabJsonValue.h"

using namespace PlayFab;

// Sets default values for this component's properties
UActorComponent_Playfab::UActorComponent_Playfab()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

		// 금칙어 리스트 > 닉네임
	static ConstructorHelpers::FObjectFinder<UDataTable> FindBadNameData(TEXT("/Game/Project/DataTable/Data_BadName"));
	if (FindBadNameData.Succeeded())
		BadNameTable = FindBadNameData.Object;
}

// Called when the game starts
void UActorComponent_Playfab::BeginPlay()
{
	Super::BeginPlay();

	// Playfab title ID
	GetMutableDefault<UPlayFabRuntimeSettings>()->TitleId = TEXT("C9B19");
	// Player Pawn 
	PlayerOwner = Cast<APawn_Player>(GetOwner());
}

// Called every frame
void UActorComponent_Playfab::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// Playfab Script // Login
// Custom Login 
void UActorComponent_Playfab::Login_Custom(const FString& customid)
{
	// 커스텀 아이디
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	ClientModels::FLoginWithCustomIDRequest request;

	request.CreateAccount = true;
	request.CustomId = customid;

	ClientAPI->LoginWithCustomID(
		request,
		UPlayFabClientAPI::FLoginWithCustomIDDelegate::CreateLambda([&](const ClientModels::FLoginResult& result) {

			PlayFabID = result.PlayFabId;
			LastLoginTime = result.LastLoginTime.mValue.ToString();

			UE_LOG(LogTemp, Log, TEXT("// custom Login Success :: %s ( %s )"), *PlayFabID, *LastLoginTime);
			// UE_LOG(LogTemp, Log, TEXT("// SessionTicket :: ( %s )"), *result.AuthenticationContext->GetClientSessionTicket());

			// 서버 접속 여부 체크
			// getServerTitleData(result.NewlyCreated);

			// 로그인 성공 // NewlyCreated 최근 생성 계정 판단
			SuccessLogin(result.NewlyCreated);

			}),
		FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// Email Login 
void UActorComponent_Playfab::Login_Email(const FString& email, const FString& pw)
{
	// 연동 아이디 처리 
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	ClientModels::FLoginWithEmailAddressRequest request;

	request.Email = email;
	request.Password = pw;

	ClientModels::FPurchaseItemRequest asd;
	
	ClientAPI->PurchaseItem(asd);


	ClientAPI->LoginWithEmailAddress(
		request,
		UPlayFabClientAPI::FLoginWithEmailAddressDelegate::CreateLambda([&](const ClientModels::FLoginResult& result) {

			PlayFabID = result.PlayFabId;
			LastLoginTime = result.LastLoginTime.mValue.ToString();
			UE_LOG(LogTemp, Log, TEXT("// EmailAddress Login Success :: %s ( %s )"), *PlayFabID, *LastLoginTime);
			// UE_LOG(LogTemp, Log, TEXT("// SessionTicket :: ( %s )"), *result.AuthenticationContext->GetClientSessionTicket());
			// getUserTitleData();

			}),
		FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}

// Playfab Script
// 플레이팹 스크립트 이벤트 호출
// // 매개변수 x 
void UActorComponent_Playfab::ScriptCustomNotField(const FString& FunctionName)
{
	ErrorFunctionName = FunctionName;
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	ClientModels::FExecuteCloudScriptRequest request;
	request.FunctionName = FunctionName;
	request.GeneratePlayStreamEvent = true;

	ClientAPI->ExecuteCloudScript(
		request,
		UPlayFabClientAPI::FExecuteCloudScriptDelegate::CreateUObject(this, &UActorComponent_Playfab::SuccessScriptEvent),
		FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
	);

}
// // 매개변수 o
void UActorComponent_Playfab::ScriptCustom(const FString& FunctionName, const FString& FieldName, const FString& value)
{
	ErrorFunctionName = FunctionName;
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	UE_LOG(LogTemp, Log, TEXT("// ScriptCustom :: %s , value :: %s "), *ErrorFunctionName, *value);

	TSharedPtr<FJsonObject> outerWrapper = MakeShareable(new FJsonObject());
	outerWrapper->SetStringField(FieldName, value);

	auto functionParameter = PlayFab::FJsonKeeper();
	functionParameter.readFromValue(MakeShareable(new FJsonValueObject(outerWrapper)));

	PlayFab::ClientModels::FExecuteCloudScriptRequest request;
	request.FunctionName = FunctionName;
	request.FunctionParameter = functionParameter;
	request.GeneratePlayStreamEvent = true;

	ClientAPI->ExecuteCloudScript(
		request,
		PlayFab::UPlayFabClientAPI::FExecuteCloudScriptDelegate::CreateUObject(this, &UActorComponent_Playfab::SuccessScriptEvent),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript));

}
// // 매개변수 배열 
void UActorComponent_Playfab::ScriptCustomArray(const FString& FunctionName, const FString& FieldName, const TArray<FString>& StringArray)
{
	ErrorFunctionName = FunctionName;
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());

	if (!JsonObj.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("// JSonObj isValid "));
		return;
	}

	TArray< TSharedPtr<FJsonValue> > EntriesArray;

	for (auto String : StringArray)
	{
		EntriesArray.Add(MakeShareable(new FJsonValueString(String)));
	}

	JsonObj->SetArrayField(FieldName, EntriesArray);

	PlayFab::ClientModels::FExecuteCloudScriptRequest request;
	request.FunctionName = FunctionName;
	request.FunctionParameter = JsonObj;
	request.GeneratePlayStreamEvent = true;

	ClientAPI->ExecuteCloudScript(
		request,
		PlayFab::UPlayFabClientAPI::FExecuteCloudScriptDelegate::CreateUObject(this, &UActorComponent_Playfab::SuccessScriptEvent),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript));

}

// 스크립트 호출 성공 이벤트 
// Playfab 외부 스크립트 호출시 리턴 출력 문구를 설정해 둠
void UActorComponent_Playfab::SuccessScriptEvent(const PlayFab::ClientModels::FExecuteCloudScriptResult& result)
{
	UE_LOG(LogTemp, Log, TEXT("// SuccessScriptEvent:: %s"), *result.FunctionName);
	FJsonValue* value = result.FunctionResult.GetJsonValue().Get();
	if (!value->IsNull())
	{
		ScriptResponseEvent(value);
	}
	else
		UE_LOG(LogTemp, Log, TEXT("// SuccessScriptEvent Not return Data"));
}

// 스크립트 호출 실패
void UActorComponent_Playfab::ErrorScript(const PlayFab::FPlayFabCppError& error)
{
	FString str = error.GenerateErrorReport();

	UE_LOG(LogTemp, Log, TEXT("// Error Script ( %s ):: %s"), *error.ErrorName, *str);

	// "Item not found" 아이템 구매시 잘못된 정보 입력
	if (error.ErrorName == "Item not found")
	{
		UE_LOG(LogTemp, Log, TEXT("// Item not found"));
		return;
	}
	// 중복 이름 , 이름 생성 조건 실패 (string 3 ~24)
	if (error.ErrorName == "NameNotAvailable" || "InvalidParams")
	{
		// CheckerCharacterName = error.ErrorName;
		bCheckerCreateChracter = false;
		// 실패시 위젯 변경점 전달 
		if (PlayerOwner)
			PlayerOwner->CustomizingWidgetNameChecker(bCheckerCreateChracter);
	}
	
	//// 아이템 구매 실패
	//// "WrongPrice" || "WrongVirtualCurrency") // ItemNotFound , StoreNotFound
	//if (error.ErrorName == "InsufficientFunds")
	//{
	//	// 돈 부족
	//	AddPopupWidget(nullptr, enum_PopupStyle::Popup1Key, enum_PopupRun::Purchage,
	//		FText::FromString(" You cannot purchase the item because you don't have enough money."), FText::FromString("Purchase failed"));
	//}
	//// 아이디 밴
	//if (error.ErrorName == "AccountBanned")
	//{
	//	// 금지 , 서버 차단
	//	// FString EmaliCheck;
	//	AddPopupWidget(nullptr, enum_PopupStyle::Popup1Key, enum_PopupRun::Cancel,
	//		FText::FromString(str + "If you want to check the Ban information, contact ad@ma.com."), FText::FromString("AccountBanned"));
	//}
}

// Playfab Response
// 플레이팹 스크립트 콜벡 이벤트
void UActorComponent_Playfab::ScriptResponseEvent(FJsonValue* value)
{
	FString Selection;

	TArray<FString> getFieldNames;
	value->AsObject().Get()->Values.GetKeys(getFieldNames);

	if (getFieldNames.Num() > 0)
		Selection = getFieldNames[0];

	// CallBack String Data
	FString getStringData = value->AsObject().Get()->GetStringField(Selection);
	UE_LOG(LogTemp, Log, TEXT("// getStringData :: %s"), *getStringData);

	// 플레이어 디폴드 데이터 업데이트
	if (Selection == "UpdataDefaultValue")
	{
		UE_LOG(LogTemp, Log, TEXT("// Playfab _ updateDefaultValue :: %s"), *getStringData);
		getUserTitleData();
	}
	else if (Selection == "Createcharacter")
	{
		UE_LOG(LogTemp, Log, TEXT("// Playfab _ Createcharacter :: %s"), *getStringData);
		// Test _ 캐릭터 생성 완료시 다음 로직 구현 처리 
		if (PlayerOwner)
			PlayerOwner->Blueprint_CreateCharacter();
	}
	else if (Selection == "SeunghunGrant")
	{
		UE_LOG(LogTemp, Log, TEXT("// Playfab _ GrantSuccess__call__getUserTitleData"));
		// 서버에서 아이템을 주고 인벤토리에 넣는다 -> 타이틀 데이터 가져오고나서 레벨이동시작
		getIngamePlayerData();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return Callback Event 

void UActorComponent_Playfab::SuccessLogin(bool NewlyCreated)
{
	// 첫 접속 여부에 따른 디폴트 데이터 추가 
	if (NewlyCreated)
	{
		UE_LOG(LogTemp, Log, TEXT("// NewlyCreated "));
		ScriptCustomNotField("UpdataDefaultValue");
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("// getUserTitleData "));
		getUserTitleData();
	}
}

void UActorComponent_Playfab::getUserTitleData(FString targetTitle)
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	PlayFab::ClientModels::FGetUserDataRequest request;

	// 찾고자 하는 타이틀 // 빈데이터 = 전체 타이틀 데이터
	TArray<FString> targetKey;
	targetKey.Add(targetTitle);

	request.Keys = targetKey;
	request.PlayFabId = PlayFabID;
	// 유저 데이터 찾기
	ClientAPI->GetUserData(
		request,
		PlayFab::UPlayFabClientAPI::FGetUserDataDelegate::CreateLambda([&](const PlayFab::ClientModels::FGetUserDataResult& result) {

			TArray<FString> getKeys;
			result.Data.GetKeys(getKeys);
			for (auto it : getKeys)
			{
				const PlayFab::ClientModels::FUserDataRecord* record = result.Data.Find(it);
				if (record)
				{
					UserTitleData.Add(it, record->Value);
					// 캐릭터 생성 여부 체크
					if (it == FString("Createcharacter"))
					{
						if (*record->Value == FString("1"))
						{
							bCheckerCreateChracter = true;
							UE_LOG(LogTemp, Log, TEXT("// Checker Create Chracter :: %d "), true);
						}
						else
						{
							bCheckerCreateChracter = false;
							UE_LOG(LogTemp, Log, TEXT("// Checker Create Chracter :: %d "), false);
						}
					}
					// 친밀도
					else if (it == FString("Fellowship"))
					{
						// Fellowship = record->Value;
						UE_LOG(LogTemp, Log, TEXT("// GetUserData() Fellowship :: %s "), *record->Value);
					}
					else
						UE_LOG(LogTemp, Log, TEXT("// GetUserData() Key ( %s ) , Value ( %s ) "), *it, *record->Value);
				}
			}
			// 캐릭터 생성 여부
			if (PlayerOwner)
				PlayerOwner->InitPlayFabUserTitleData(bCheckerCreateChracter);

			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// 인게임 접속시 플레이어 데이터 로드 
// 로딩 구성 추가 하기. 테스트 버전 딜레이 임시 추가
// PlayerOwner->Check_getIngameLoadingCount()
void UActorComponent_Playfab::getIngamePlayerData()
{
	// Check_getIngameLoadingCount :: 게임 정보 데이터를 불러오는 함수 수량 체크
	// Test Count :: 2
	if (PlayerOwner)
		PlayerOwner->Test_LoadingCount = 2;

	// Test_LoadingCount Check :: o
	// 인벤토리 정보
	getInventoryList();
	// UserCharacterName // Playfab Display Name 체크
	getUserTitleName();

	// Test_LoadingCount Check :: x
	// 업적 데이터 체크
	getStatisticsEvent();
	// 공지 정보 체크
	getNoticeEvent(3);

	// updateUserTitleData(UserTitleData);
}
// 유저 닉네임
void UActorComponent_Playfab::getUserTitleName()
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	PlayFab::ClientModels::FGetAccountInfoRequest request;
	request.PlayFabId = PlayFabID;

	ClientAPI->GetAccountInfo(
		request,
		PlayFab::UPlayFabClientAPI::FGetAccountInfoDelegate::CreateLambda([&](const PlayFab::ClientModels::FGetAccountInfoResult& result) {

			UserCharacterName = result.AccountInfo.Get()->TitleInfo->DisplayName;
			UE_LOG(LogTemp, Log, TEXT("// getAccountInfo titleName :: %s "), *UserCharacterName);
			if (PlayerOwner)
				PlayerOwner->Check_getIngameLoadingCount();
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// 아이템 인벤토리 
void UActorComponent_Playfab::getInventoryList()
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	PlayFab::ClientModels::FGetUserInventoryRequest request;

	ClientAPI->GetUserInventory(
		request,
		PlayFab::UPlayFabClientAPI::FGetUserInventoryDelegate::CreateLambda([&](const PlayFab::ClientModels::FGetUserInventoryResult& result) {

			InventoryProperty.Reset();
			UE_LOG(LogTemp, Log, TEXT("// getInventoryList :: Len ( %d )"), result.Inventory.Num());

			for (auto it : result.Inventory)
			{
				FItemproperty Invendata;
				Invendata.ItemId = it.ItemId;
				Invendata.ItemInstanceId = it.ItemInstanceId;
				Invendata.ItemClass = it.ItemClass;
				Invendata.RemainingUses = it.RemainingUses;
				Invendata.UnitPrice = it.UnitPrice;

				// 아이템 컬러 정보 저장 방법 결정이 필요.
				// 아이템 색상 정보를 테이블과 데이터 처리 방법? 
				/*if (it.CustomData.Contains("Color"))
				{
					FString* colorData = it.CustomData.Find("Color");
					Invendata.colorData = colorData[0];
					UE_LOG(LogTemp, Log, TEXT("// getInventoryList :: Color ( %s )"), *Invendata.colorData);
				}
				else
					UE_LOG(LogTemp, Log, TEXT("// Error : getInventoryList(id : %s ) Not Find ColorData"), *it.ItemId);*/

				// 커스터마이지 관련 데이터 저장 구분.
				/*if (CheckUsertitleData("Body", Invendata)) {

					TTuple<FName, FName> ChangeBody(FName(*Invendata.ItemId), FName(*Invendata.colorData));
					CustomData.Body = ChangeBody;
				}
				else if (CheckUsertitleData("Eyebrow", Invendata)) {
					TTuple<FName, FName> ChangeBody(FName(*Invendata.ItemId), FName(*Invendata.colorData));
					CustomData.Eyebrow = ChangeBody;
				}
				else if (CheckUsertitleData("Eyes", Invendata)) {
					TTuple<FName, FName> ChangeBody(FName(*Invendata.ItemId), FName(*Invendata.colorData));
					CustomData.Eyes = ChangeBody;
				}
				else if (CheckUsertitleData("Hair", Invendata)) {
					TTuple<FName, FName> ChangeBody(FName(*Invendata.ItemId), FName(*Invendata.colorData));
					CustomData.Hair = ChangeBody;
				}
				else if (CheckUsertitleData("Lips", Invendata)) {
					TTuple<FName, FName> ChangeBody(FName(*Invendata.ItemId), FName(*Invendata.colorData));
					CustomData.Lips = ChangeBody;
				}*/
				// 인벤토리 아이템 정보 리스트
				InventoryProperty.Add(Invendata);

				// 인벤토리 위젯 슬롯 추가 함수 연결
				// updateInventoryItem(Invendata);
			}

			if (result.VirtualCurrency.Contains("GC"))
			{
				const int* findVC = result.VirtualCurrency.Find("GC");
				if (PlayerOwner)
				{
					PlayerOwner->VirtualCoin = FString::FromInt(findVC[0]);
					PlayerOwner->updateInventoryCoin();
				}
			}

			// 로딩 카운터 체크 후 다음 레벨 진행 여부 확인.
			if (PlayerOwner)
				PlayerOwner->Check_getIngameLoadingCount();
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// 넥네임 금칙어 체크하기
bool UActorComponent_Playfab::FindCheckBadName(const FString& name)
{
	TArray<FName> outputBadNameCheckRow = BadNameTable->GetRowNames();

	// 닉네임 구조 공백문자 x
	for (auto it : outputBadNameCheckRow)
	{
		FBadNameTable* data = BadNameTable->FindRow<FBadNameTable>(*it.ToString(), "");
		int32 Bad = name.Find(data->BadString);
		if (Bad >= 0)
		{
			UE_LOG(LogTemp, Log, TEXT("// Bad String Find Point Number :: %d "), Bad);
			return true;
		}
	}
	return false;
}
// 유저 닉네임 업데이트 (중복 체크, 변경)
void UActorComponent_Playfab::updateUserTitleName(const FString& DisplayName)
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	PlayFab::ClientModels::FUpdateUserTitleDisplayNameRequest request;
	request.DisplayName = DisplayName;

	if (FindCheckBadName(DisplayName))
	{
		// 금칙어 처리 추가 하기.
		if (PlayerOwner)
			PlayerOwner->updateDisplayNameEvent(false);
		UE_LOG(LogTemp, Error, TEXT("// Forbidden Name :: %s "), *DisplayName);
		return;
	}

	ClientAPI->UpdateUserTitleDisplayName(
		request,
		PlayFab::UPlayFabClientAPI::FUpdateUserTitleDisplayNameDelegate::CreateLambda([&](const PlayFab::ClientModels::FUpdateUserTitleDisplayNameResult& result) {

			UserCharacterName = result.DisplayName;
			bCheckerCreateChracter = true;

			UE_LOG(LogTemp, Log, TEXT("// Success Create DisplayName :: %s "), *result.DisplayName);
			
			if (PlayerOwner)
				PlayerOwner->updateDisplayNameEvent(bCheckerCreateChracter);

			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
	);
}
// 업적 데이터 getStatisticsEvent
// 미션 데이터 & 통계(랭킹) 데이터 
void UActorComponent_Playfab::getStatisticsEvent()
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	PlayFab::ClientModels::FGetPlayerStatisticsRequest request;

	ClientAPI->GetPlayerStatistics(
		request,
		PlayFab::UPlayFabClientAPI::FGetPlayerStatisticsDelegate::CreateLambda([&](const PlayFab::ClientModels::FGetPlayerStatisticsResult& result) {

			TMap<FString, int> MissionData;
			for (auto it : result.Statistics)
			{
				MissionData.Add(it.StatisticName, it.Value);
			}
			// 업적 데이터 관리
			// updateMissionEvent(MissionData);

			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
		);
}
// 공지 정보 가져오기 NoticeCount 가져올 공지 수
void UActorComponent_Playfab::getNoticeEvent(int NoticeCount)
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	PlayFab::ClientModels::FGetTitleNewsRequest request;
	request.Count = NoticeCount;

	ClientAPI->GetTitleNews(
		request,
		PlayFab::UPlayFabClientAPI::FGetTitleNewsDelegate::CreateLambda([&](const PlayFab::ClientModels::FGetTitleNewsResult& result) {

			for (auto it : result.News)
			{
				// 공지 팝업을 위한 정보 처리
				// 제목 // 내용
				// updateNotice(it.Title, it.Body);
				UE_LOG(LogTemp, Log, TEXT("// getNotice Body :: %s "), *it.Body);
			}
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}

/////////////////////////////////Grant -> getinGamePlayerData -> level이동
// 승훈 업로드 코스튬, Grant 
void UActorComponent_Playfab::UploadMyCustom(const FString& FunctionName, const FString& FieldName,const TArray<int> ItemIDs)
{
	TArray<FString> FirstCostumeData;

	for (auto it : ItemIDs)
	{
		FirstCostumeData.Push(FString::FromInt(it));
	}
	// 커스텀 데이터 일단 저장

	ScriptCustomArray(FunctionName, FieldName, FirstCostumeData);
}

void UActorComponent_Playfab::getStoreItemList(const FString& CatalogVersion, const FString& StoreID)
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
				PlayerOwner->UpdateStore(ShopDatas);
			}
			//PlayerOwner->Seunghun_ShopUpdate(ShopDatas);

			}// 에러함수 (넣지않음)
	));


}
