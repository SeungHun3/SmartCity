// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent_Playfab.h"
#include "Pawn_Player.h"
#include "Engine/Engine.h"
#include "Widget/Checking_Folder/Widget_CheckingAttandance_Main.h"
#include "GameInstance_Solaseado.h"

#include "Core/PlayFabClientAPI.h"
#include "Misc/DateTime.h"

#include "PlayFabJsonObject.h"
#include "PlayFabJsonValue.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"


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
	//// 아이디 밴
	//if (error.ErrorName == "AccountBanned")
	//{
	//	// 금지 , 서버 차단
	//	// FString EmaliCheck;
	//	AddPopupWidget(nullptr, enum_PopupStyle::Popup1Key, enum_PopupRun::Cancel,
	//		FText::FromString(str + "If you want to check the Ban information, contact ad@ma.com."), FText::FromString("AccountBanned"));
	//}
}

//////////////////////////////////////////////////////
// 출석체크 
// 오늘 체크했는지 확인여부
bool UActorComponent_Playfab::Is_Today_Checked()
{
	if (PlayFab_Statistics.Contains("Is_Checked_Daily")) // 플레이팹 리더보드에서 오늘 체크를 했는지 확인 후에
	{
		int32 isChecked = *PlayFab_Statistics.Find("Is_Checked_Daily");
		// 오늘 체크를 했다면 = true, 안했다면 false
		if (isChecked == 1)
		{
			return true;
		}
		else { return false; }
		
	}
	else //Is_Checked_Daily의 키가 없다면
	{
		ScriptCustomNotField("Default_Statistics");
		UE_LOG(LogTemp, Log, TEXT("// NoData : Is_Checked_Daily"));
		return false;

	}
}
// 오늘까지 체크한 출석일수
int UActorComponent_Playfab::Get_Checking_Count()
{
	if (PlayFab_Statistics.Contains("Month_Reward_Count")) // 출석 카운트 데이터 가져와서 반환
	{
		return *PlayFab_Statistics.Find("Month_Reward_Count");
	}
	else // 출석 데이터가 없다면 
	{
		ScriptCustomNotField("Default_Statistics");
		UE_LOG(LogTemp, Log, TEXT("// NoData : Month_Reward_Count"));
		return 0;
	}
}

//PlayFab에서 daily check 업데이트 + checkCount ++ -> return checkCount 
void UActorComponent_Playfab::Update_Check_Change(UWidget_CheckingAttandance_Main* Widget, int coin)
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI(); 
	TSharedPtr<FJsonObject> outerWrapper = MakeShareable(new FJsonObject());
	outerWrapper->SetNumberField("Coin", coin);
	auto functionParameter = PlayFab::FJsonKeeper();
	functionParameter.readFromValue(MakeShareable(new FJsonValueObject(outerWrapper)));
	
	ClientModels::FExecuteCloudScriptRequest request;
	request.FunctionName = "Update_Attandance";
	request.GeneratePlayStreamEvent = true;
	request.FunctionParameter = functionParameter;

	ClientAPI->ExecuteCloudScript(
		request,
		UPlayFabClientAPI::FExecuteCloudScriptDelegate::CreateLambda([&, Widget](const PlayFab::ClientModels::FExecuteCloudScriptResult& result)
			{
				TSharedPtr<FJsonObject> result_Object = result.FunctionResult.GetJsonValue()->AsObject();
				TArray<FString> Keys;
				result_Object->Values.GetKeys(Keys); // 리턴값 하나의 Key 만 보냈음


				if ((Keys[0] == "Month_Reward_Count")&& result_Object->Values.Contains(Keys[0]))
				{
						int CheckingCount = static_cast<int>(result_Object->Values.Find(Keys[0])->Get()->AsNumber()); // 리턴받은 Count값
						
						if (PlayFab_Statistics.Contains(Keys[0]) && PlayFab_Statistics.Find(Keys[0]))
						{
							PlayFab_Statistics.Add(Keys[0], CheckingCount); // Map 데이터 변경시키고
							// 위젯에 전달
							Widget->ChangeSlot();
						}
				}
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// 20일 보상
void UActorComponent_Playfab::Daily_20th_Reward(UWidget_CheckingAttandance_Main* Widget, int Coin)
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	TSharedPtr<FJsonObject> outerWrapper = MakeShareable(new FJsonObject());
	outerWrapper->SetNumberField("Coin", Coin);
	auto functionParameter = PlayFab::FJsonKeeper();
	functionParameter.readFromValue(MakeShareable(new FJsonValueObject(outerWrapper)));

	ClientModels::FExecuteCloudScriptRequest request;
	request.FunctionName = "Coin_Grant";
	request.GeneratePlayStreamEvent = true;
	request.FunctionParameter = functionParameter;

	ClientAPI->ExecuteCloudScript(
		request,
		UPlayFabClientAPI::FExecuteCloudScriptDelegate::CreateLambda([&, Widget](const PlayFab::ClientModels::FExecuteCloudScriptResult& result)
			{
				//서버에 코인주고나서 위젯함수 호출
				Widget->ChangeSpecial();
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);

}


// 디버깅용 출석체크 초기화
void UActorComponent_Playfab::Debug_Clear_Attandance(UWidget_CheckingAttandance_Main* Widget)
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	ClientModels::FExecuteCloudScriptRequest request;
	request.FunctionName = "Clear_Attandance";
	request.GeneratePlayStreamEvent = true;
	ClientAPI->ExecuteCloudScript(
		request,
		UPlayFabClientAPI::FExecuteCloudScriptDelegate::CreateLambda([&, Widget](const PlayFab::ClientModels::FExecuteCloudScriptResult& result)
			{
				PlayFab_Statistics.Add("Month_Reward_Count", 0);
				PlayFab_Statistics.Add("Is_Checked_Daily", 0);

				//서버에 코인주고나서 위젯함수 호출
				Widget->Debug_Finished_Clear();
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// 디버기용 오늘 출석내용 초기화
void UActorComponent_Playfab::Debug_Clear_Daily(UWidget_CheckingAttandance_Main* Widget)
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	ClientModels::FExecuteCloudScriptRequest request;
	request.FunctionName = "Clear_CheckDaily";
	request.GeneratePlayStreamEvent = true;
	ClientAPI->ExecuteCloudScript(
		request,
		UPlayFabClientAPI::FExecuteCloudScriptDelegate::CreateLambda([&, Widget](const PlayFab::ClientModels::FExecuteCloudScriptResult& result)
			{
				PlayFab_Statistics.Add("Is_Checked_Daily", 0);

				//서버에 코인주고나서 위젯함수 호출
				Widget->Debug_Finished_DailyClear();
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}


//////////////////////////////////////////////////////
// 출석체크 끝




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
	else if (Selection == "Create_Grant")
	{
		UE_LOG(LogTemp, Log, TEXT("// Playfab _ GrantSuccess__call__getUserTitleData"));
		// 서버에서 아이템을 주고 인벤토리에 넣고 타이틀 데이터로 적용 -> 타이틀 데이터 가져오고나서 레벨이동시작
		//getIngamePlayerData();
		getUserTitleData();
	}
	else if (Selection == "updateEquipmentItem")
	{
		for (auto it : InventoryProperty)
		{
			if (it.ItemInstanceId == getStringData)
			{
				UserTitleData.Add(it.ItemClass, getStringData);
				break;
			}
		}

		//아이템 장비 성공 이벤트.
		UE_LOG(LogTemp, Log, TEXT("// Playfab _ updateEquipmentItem :: %s"), *getStringData);
		if (PlayerOwner)
			PlayerOwner->Blueprint_UpdateEquipmentItem(getStringData);
	}

	else if (Selection == "AddFriend")
	{
		getFriendListEvent();
	}
	else if (Selection == "RemoveFriend")
	{
		getFriendListEvent();
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
					else {
						UE_LOG(LogTemp, Log, TEXT("// GetUserData() Key ( %s ) , Value ( %s ) "), *it, *record->Value);
					}
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
	// Test Count :: 1
	if (PlayerOwner)
		PlayerOwner->Test_LoadingCount = 1;

	// Test_LoadingCount Check :: o
	// UserCharacterName // Playfab Display Name 체크
	getUserTitleName();

	// Test_LoadingCount Check :: x
	// 인벤토리 정보
	getInventoryList();
	// 업적,퀘스트 데이터 체크
	getStatisticsEvent();
	// 퀘스트 데이터 체크
	//GetMyQuestToServer("Tutorial");
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
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("// getAccountInfo titleName :: %s "), *UserCharacterName));
			}
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
					PlayerOwner->updateInventory();
				}
			}

			// 로딩 카운터 체크 후 다음 레벨 진행 여부 확인. // 로딩 카운터 제거 
			/*if (PlayerOwner)
				PlayerOwner->Check_getIngameLoadingCount();*/
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
			// 변수에 통계데이터 채워넣기
			for (auto it : result.Statistics)
			{
				PlayFab_Statistics.Add(it.StatisticName, it.Value);
			}
			CheckQuestInfo();
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
// FunctionName : Create_Grant  == 코스튬 플레이팹에 업로드 
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

//퀘스트 구조체 찾기 못찾았다면 -1
int UActorComponent_Playfab::FindQuestInfo_Index(const FString& QuestName)
{
	int Index = -1;
	for (int i = 0; i < MyQuest_Info.Num(); i++)
	{	// 정보가지고 있다면 
		if (MyQuest_Info[i].Quest_Name == QuestName) { Index = i;	break; }
	}

	return Index;
}
// 퀘스트 PlayFab_Statistics 없음, value 0 = 시작가능, value 1 = 진행중, value 2 = 완료
void UActorComponent_Playfab::CheckQuestInfo()
{
	TArray<FString> QuestKey;
	PlayFab_Statistics.GetKeys(QuestKey); 
	for (auto it : QuestKey)
	{	// 업적데이터중 퀘스트를 추려서  value가 1인것들만 == 진행중인 퀘스트만 담기
		if ((it.Left(5) == "Quest") && (UserTitleData.Contains(it)) && (1 == *PlayFab_Statistics.Find(it))) 
		{
			FString val = *UserTitleData.Find(it);
			FQuest_Info Quest = MakeQuestInfo(it, val);
			MyQuest_Info.Add(Quest); //데이터 넣어주기
		}
	}
}
// 퀘스트 JSon구조 == QuestName : ParseData{ 진행단계+index(Key) : 완료여부(Value),  진행단계+index(Key) : 완료여부(Value)... }
FQuest_Info UActorComponent_Playfab::MakeQuestInfo(const FString& QuestName, const FString& JsonParseData) //ex) ParseData = {"1/n0":true,"1/n1":false}
{
	//json데이터 변환
	FQuest_Info Quest;
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonParseData);
	FJsonSerializer::Deserialize(JsonReader, JsonObject);
	TArray<FString> StepKeys;
	JsonObject->Values.GetKeys(StepKeys);


	// 퀘스트 이름세팅
	Quest.Quest_Name = QuestName;
		TArray<FString> StepParse;
	StepKeys[0].ParseIntoArray(StepParse, TEXT("/n"));
	//퀘스트 진행도
	Quest.Quest_Step = FCString::Atoi(*StepParse[0]);
	
	//진행도 부분 클리어체크
	int indexSize = JsonObject->Values.Num();
	
	for (int i = 0; i < indexSize; i++)
	{
		FString KeyIndex = StepParse[0] + FString("/n") + FString::FromInt(i);
		Quest.IsFinished.Add(JsonObject->Values.Find(KeyIndex)->Get()->AsBool());
	}

	return Quest;
}

UDataTable* UActorComponent_Playfab::FindQuestTable(const FString& QuestName)
{
	UGameInstance_Solaseado* MyInstance = Cast<UGameInstance_Solaseado>(GetWorld()->GetGameInstance());
	UDataTable* Quest_Table = MyInstance->GetQuestTables();
	FQuest_List* CurrQuest = Quest_Table->FindRow<FQuest_List>(FName(*QuestName), "FindQuestTable_NoQuest");
	if (CurrQuest == nullptr)	{ return nullptr;	}

	return CurrQuest->QuestData;
}
// 퀘스트 UserTitleData Update
void UActorComponent_Playfab::Quest_Update_Title(const FString& QuestName)
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	TSharedPtr<FJsonObject> JsonSubObj = MakeShareable(new FJsonObject());
	TArray< TSharedPtr<FJsonValue> > EntriesArray;

	EntriesArray.Add(MakeShareable(new FJsonValueString(QuestName))); // 퀘스트이름
	int CurrQuest = FindQuestInfo_Index(QuestName);
	if (CurrQuest == -1)
	{
		UE_LOG(LogTemp, Log, TEXT("// Invalid_Struct "));
		return;
	}

	FString Step = FString::FromInt(MyQuest_Info[CurrQuest].Quest_Step);

	for (int i = 0; i < MyQuest_Info[CurrQuest].IsFinished.Num(); i++)
	{
		FString StepIndex = Step + FString("/n") + FString::FromInt(i);			// 퀘스트 진행도 
		JsonSubObj->SetBoolField(StepIndex, MyQuest_Info[CurrQuest].IsFinished[i]);// 퀘스트 완료여부
	}
	EntriesArray.Add(MakeShareable(new FJsonValueObject(JsonSubObj))); // 퀘스트 정보  // Key(진행도 + /n + 퀘스트index ) :Value (완료여부)

	JsonObj->SetArrayField("Quest", EntriesArray);


	//서버로 전송
	PlayFab::ClientModels::FExecuteCloudScriptRequest request;
	request.FunctionName = "UpdateQuest";
	request.FunctionParameter = JsonObj;
	request.GeneratePlayStreamEvent = true;
	ClientAPI->ExecuteCloudScript(
		request,
		UPlayFabClientAPI::FExecuteCloudScriptDelegate::CreateLambda([&](const PlayFab::ClientModels::FExecuteCloudScriptResult& result)
			{
				
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);

}

void UActorComponent_Playfab::Quest_Update_Statistic(const FString& QuestName, enum_Quest_Update Update)
{
	const enum_Quest_Update QuestCondition = Update;
	int JsonVal = static_cast<int>(QuestCondition);


	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	TArray< TSharedPtr<FJsonValue> > EntriesArray;
	EntriesArray.Add(MakeShareable(new FJsonValueString(QuestName)));
	EntriesArray.Add(MakeShareable(new FJsonValueNumber(JsonVal)));
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	JsonObj->SetArrayField("statistic", EntriesArray);

	PlayFab::ClientModels::FExecuteCloudScriptRequest request; // 업적데이터에 퀘스트 Value 2로 넣어주기
	request.FunctionName = "Update_Statistic";
	request.FunctionParameter = JsonObj;
	request.GeneratePlayStreamEvent = true;
	ClientAPI->ExecuteCloudScript(
		request,
		UPlayFabClientAPI::FExecuteCloudScriptDelegate::CreateLambda([&, QuestName, QuestCondition](const PlayFab::ClientModels::FExecuteCloudScriptResult& result)
			{
				FQuest_Info Quest;
				int checkIndex;
				int ENUM_Val = static_cast<int>(QuestCondition);
				switch (QuestCondition)
				{
				case enum_Quest_Update::Drop:// 퀘스트포기

					PlayFab_Statistics.Add(QuestName, ENUM_Val); // 업적데이터 0 으로 변경
					for (int i = 0; i < MyQuest_Info.Num(); i++) // 퀘스트 데이터지우기
					{
						if (PlayFab_Statistics.Contains(QuestName)) { PlayFab_Statistics.Add(QuestName, 0); }
						if (MyQuest_Info[i].Quest_Name == QuestName) { MyQuest_Info.RemoveAt(i);	break; }
					}
					PlayerOwner->Quest_Drop(QuestName);
					break;


				case enum_Quest_Update::Start:	//퀘스트 시작

					PlayFab_Statistics.Add(QuestName, ENUM_Val); // 업적데이터 추가
					checkIndex = FindQuestInfo_Index(QuestName);					// 초기화
					if (checkIndex != -1) { MyQuest_Info.RemoveAt(checkIndex); }	//동일이름의 퀘스트명이 있다면 지우고
					Quest = SetQuestInfo(QuestName, 1); // 퀘스트 시작정보 구조체 담아서
					MyQuest_Info.Add(Quest); // 배열에 추가 후 
					Quest_Update_Title(QuestName); // 타이틀 데이터에 업데이트하기

					PlayerOwner->Quest_Start(Quest);
					break;

				case enum_Quest_Update::Complete:
					// 보상받기 // 받으면 타이틀 데이터 지우기
					for (int i = 0; i < MyQuest_Info.Num(); i++) // 퀘스트 데이터지우기
					{
						if (PlayFab_Statistics.Contains(QuestName)) { PlayFab_Statistics.Add(QuestName, ENUM_Val); }
						if (MyQuest_Info[i].Quest_Name == QuestName) { MyQuest_Info.RemoveAt(i);	break; }
					}
					PlayerOwner->Quest_Complete(QuestName);
					break;
				}

			}));

}

FQuest_Info UActorComponent_Playfab::SetQuestInfo(const FString& QuestName, int Step)
{
	FQuest_Info Quest;
	Quest.Quest_Name = QuestName;	// 퀘스트 이름
	Quest.Quest_Step = Step;		// 퀘스트 진행도
	Quest.IsFinished.Reset();

	UDataTable* MyQuestTable = FindQuestTable(QuestName);
	TArray<int> RowNames = GetQuestRowNames(FString::FromInt(Step), MyQuestTable);
	int IndexSize = RowNames.Num();
	if (IndexSize == 0) { return Quest; } // 진행도가 없다면 Quest.IsFinished.Reset() 만 시키고 탈출

	for (int i = 0; i < IndexSize; i++)
	{
		Quest.IsFinished.Add(false); // 퀘스트 완료여부
	}

	//퀘스트(선택된데이터테이블)가 가지고 있는 데이터테이블(대화시스템)
	FString TablePropName = FString::FromInt(RowNames[0]); // 같은 진행도를 가진 인덱스중 첫번째의 데이터테이블
	FQuest_Info CurrQuestTable = *MyQuestTable->FindRow<FQuest_Info>(FName(*TablePropName), "NoData_Questinfo_Table");
	Quest.QuestTable = CurrQuestTable.QuestTable;

	return Quest;
}

//같은 진행도를 가진 테이블 행 숫자뽑아서 배열가져오기 // 프로퍼티 Quest_Step 을 진행도로 탐색 (FQuest_Info Quest; Quest.Quest_Step;)
TArray<int> UActorComponent_Playfab::GetQuestRowNames(const FString& QuestStepProp, UDataTable* QuestTable)
{
	TArray<FString> RowNames;
	TArray<int> QuestRowNames;
	RowNames = UDataTableFunctionLibrary::GetDataTableColumnAsString(QuestTable, FName("Quest_Step"));

	for (int i = 0; i < RowNames.Num(); i++)
	{
		if (RowNames[i] == QuestStepProp)
		{
			QuestRowNames.Add(i + 1);
		}
	}
	return QuestRowNames;
}
// 퀘스트 총 진행도구하기
int UActorComponent_Playfab::QuestTotalStepcount(const FString& QuestName)
{
	UDataTable* CheckTable = FindQuestTable(QuestName);
	TArray<FName> checkRows = CheckTable->GetRowNames(); // 모든행을 구해서
	TArray<int> Totalindex;
	for (auto it : checkRows)
	{
		FQuest_Info* checkProp = CheckTable->FindRow<FQuest_Info>(it, "TotalStepCount, Nodata");

		if (checkProp) 	{	Totalindex.AddUnique(checkProp->Quest_Step); 	}	// 프로퍼티 AddUnique로 추가 (중복없이넣기)
	}
	
	return Totalindex.Num();
}

void UActorComponent_Playfab::Quest_Start(const FString& QuestName)
{
	Quest_Update_Statistic(QuestName, enum_Quest_Update::Start);
}

bool UActorComponent_Playfab::Quest_Finish(const FString& QuestName, int index)
{
	int CurrQuest = FindQuestInfo_Index(QuestName);
	if (CurrQuest == -1)												{	return false;	}	// 잘못된 이름이거나
	if (!MyQuest_Info[CurrQuest].IsFinished.IsValidIndex(index))	{	return false; }	// 잘못된 인덱스 접근이라면

	MyQuest_Info[CurrQuest].IsFinished[index] = true;
	Quest_Update_Title(QuestName);

	for (int i = 0; i < MyQuest_Info[CurrQuest].IsFinished.Num(); i++)
	{
		if (!MyQuest_Info[CurrQuest].IsFinished[i]) { return false; } // index가 전부 true가 아니라면
	}

	//Step의 index가 모두 true라면
	return true;
}

void UActorComponent_Playfab::Quest_Next(const FString& QuestName)
{
	int CurrQuest = FindQuestInfo_Index(QuestName);
	if (CurrQuest == -1)	{  return; }	// 잘못된 이름이라면 탈출

	int NextStep = MyQuest_Info[CurrQuest].Quest_Step + 1;

	FQuest_Info Quest;
	Quest = SetQuestInfo(QuestName, NextStep);
	// 퀘스트가 끝났다면						// 퀘스트 완료 업적 업데이트
	if (!Quest.IsFinished.IsValidIndex(0))		{	Quest_Complete(QuestName);	return;	}

	MyQuest_Info[CurrQuest] = Quest; //정보담고
	PlayerOwner->Quest_Next(MyQuest_Info[CurrQuest]);
	Quest_Update_Title(QuestName);		// 타이틀 데이터에 업데이트하기
}
//퀘스트포기 == 업적데이터 0로 바꿔주기
void UActorComponent_Playfab::Quest_Drop(const FString& QuestName)
{
	Quest_Update_Statistic(QuestName, enum_Quest_Update::Drop);
}
//퀘스트완료 == 업적데이터 2로 바꿔주기
void UActorComponent_Playfab::Quest_Complete(const FString& QuestName)
{
	Quest_Update_Statistic(QuestName, enum_Quest_Update::Complete);
}

bool UActorComponent_Playfab::Quest_Check_isDoing(const FString& QuestName)
{
	for (auto it : MyQuest_Info)
	{
		if (it.Quest_Name == QuestName)	{	return true;	}
	}
	return false;
}

bool UActorComponent_Playfab::Quest_Check_IsComplete(const FString& QuestName)
{
	TArray<FString> QuestKey;
	PlayFab_Statistics.GetKeys(QuestKey);
	for (auto it : QuestKey)
	{
		if ((it == QuestName) && (2 == *PlayFab_Statistics.Find(it))) 	{	return true;}	// 업적데이터중 퀘스트이름을 찾아서  value가 2인것들만
	}

	//못찾으면 false
	return false;
}

bool UActorComponent_Playfab::Quest_Check_IsLastFinished(const FString& QuestName)
{
	int FindQuest = FindQuestInfo_Index(QuestName);
	if (FindQuest == -1) {  return  false; }
	
	int StepTotalCount = QuestTotalStepcount(QuestName);

	if (MyQuest_Info[FindQuest].Quest_Step >= StepTotalCount) // 진행도가 최대테이블을 넘어서는 경우 완료했는지 파악하기
	{
		for (auto it : MyQuest_Info[FindQuest].IsFinished)
		{
			if (!it) {  return false; }// 하나라도 퀘스트를 완료하지 못한상태라면 false탈출
		}
		return true; //모두완료했다면 true 탈출
	} 

	//진행도가 부족한경우
	return false;
}

enum_Quest_Condition UActorComponent_Playfab::Quest_Check_Condition(const FString& QuestName)
{
	if (Quest_Check_isDoing(QuestName))
	{
		if (Quest_Check_IsLastFinished(QuestName))//마지막 보상받기 까지 왔는지여부
		{
			return enum_Quest_Condition::LastFinished;
		}
		return enum_Quest_Condition::IsDoing;//그냥 진행중일때
	}
	else if (Quest_Check_IsComplete(QuestName))	{	return	enum_Quest_Condition::IsComplete;	} // 보상까지 받고 끝난상태일때

	return enum_Quest_Condition::NotStart;
}


//업적 데이터 서버로 업데이트하기
void UActorComponent_Playfab::UpdateAchievement(int AchieveNumber)
{
	FString Temp;
	FString AchieveCount = FString::FromInt(AchieveNumber); // AchieveNumber = 001~999 Naming 맞추기용
	for (int i = 0; i < 3 - AchieveCount.Len(); i++)
	{
		Temp += "0";
	}
	AchieveCount = Temp + AchieveCount;
	
	FString FullName = AchieveName + AchieveCount;
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	JsonObj->SetStringField("name", FullName);
	PlayFab::ClientModels::FExecuteCloudScriptRequest request;
	request.FunctionName = "Upload_StatisticName";
	request.FunctionParameter = JsonObj;
	request.GeneratePlayStreamEvent = true;
	ClientAPI->ExecuteCloudScript(
		request,
		UPlayFabClientAPI::FExecuteCloudScriptDelegate::CreateLambda([&, FullName, AchieveCount](const PlayFab::ClientModels::FExecuteCloudScriptResult& result)
			{
				//서버업데이트 끝날때 호출
				PlayFab_Statistics.Add(FullName, 0);
				PlayerOwner->Finished_UpdateAchieve(AchieveCount);
			}));
			
}
//업적데이터만 추려서 반환
TArray<FString> UActorComponent_Playfab::GetAchievement()
{
	TArray<FString> AchieveNumber;
	TArray<FString> Keys;
	PlayFab_Statistics.GetKeys(Keys);
	for (auto it : Keys)
	{
		if (it.Find(AchieveName) == 0) 
		{
			AchieveNumber.Add(it.Right(it.Len() - AchieveName.Len()));
		}
	}
	return AchieveNumber;
}

void UActorComponent_Playfab::Debug_ClearAchievement()
{
	TArray<FString> Keys, RemoveKeys;
	PlayFab_Statistics.GetKeys(Keys);

	for (auto it : Keys)
	{
		if (it.Find(AchieveName) == 0)
		{
			RemoveKeys.Add(it);
			//PlayFab_Statistics.Remove(it);

		}
	}
	PlayFab_Statistics.GetKeys(Keys);
	for (auto it : Keys)
	{
		UE_LOG(LogTemp, Log, TEXT("// statistics Key :: %s "), *it);
		// 통계정보에서 클리어하는 방법 확인해보기
	}
}

////////////////////////////////////////////////
//Friend
//친구 리스트 업데이트
void UActorComponent_Playfab::getFriendListEvent()
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	
	//친구 계정들의 프로필을 받기 위해서 이렇게 따로 설정해줘야 한다.
	//PlayFab의 타이틀 설정에서도 해제해줘야한다.
	TSharedPtr<PlayFab::ClientModels::FPlayerProfileViewConstraints> ProfileConstraints(new PlayFab::ClientModels::FPlayerProfileViewConstraints);
	ProfileConstraints->ShowCreated=true;
	ProfileConstraints->ShowDisplayName = true;
	ProfileConstraints->ShowLastLogin = true;
	ProfileConstraints->ShowLocations = true;

	PlayFab::ClientModels::FGetFriendsListRequest request;
	request.ProfileConstraints = ProfileConstraints;

	
	//PlayFab::UPlayFabClientAPI::FGetFriendsListDelegate::;
	ClientAPI->GetFriendsList(
		request,
		PlayFab::UPlayFabClientAPI::FGetFriendsListDelegate::CreateUObject(this,&UActorComponent_Playfab::UpdateFriendList),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}

void UActorComponent_Playfab::UpdateFriendList(const PlayFab::ClientModels::FGetFriendsListResult& result)
{
	bool bBlock = false;
	FriendList.Empty();
	for (auto it : result.Friends)
	{
		//if (it.Tags.Num() > 0)
		//	bBlock = true;

		//it.Profile->L
		//updateFriendsList(it.TitleDisplayName, it.FriendPlayFabId, bBlock);
		//it.Profile
		if (true /* 여기에 한국인지 체크*/)
		{
			UE_LOG(LogTemp, Log, TEXT("// UpdateFriendList :: city : %s , time %d : %d "), *it.Profile->Locations[0].City, it.Profile->LastLogin.mValue.UtcNow().GetHour()+9%24, it.Profile->LastLogin.mValue.UtcNow().GetMinute());

			
			FFriendStruct sTemp;
			sTemp.LastLogin= it.Profile->LastLogin.mValue;
			sTemp.PlayFabID= it.Profile->PlayerId;
			sTemp.TitleID= it.Profile->DisplayName;
			FriendList.Add(sTemp.TitleID, sTemp);
			
		}
	}

	UpdateFriend.Broadcast();
}

void UActorComponent_Playfab::getProfileEvent()
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	//ClientAPI->UpdateAvatarUrl();
	//FUpdateCharacterDataRequest;
	//PlayFab::ClientModels::FUpdateAvatarUrlRequest;
	//FUpdateCharacterDataRequest();

	PlayFab::ClientModels::FGetFriendsListRequest request;
	request.ProfileConstraints->ShowCreated = true;



	//for (auto it : FriendList)
	//{
	//	if (it.Value.TitleID != "sdsd")
	//	{
	//
	//	}
	//}
}

void UActorComponent_Playfab::UpdateLoginTimeEvent()
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	PlayFab::ClientModels::FUpdateUserDataRequest request;


	ClientAPI->UpdateUserData(
		request,
		PlayFab::UPlayFabClientAPI::FUpdateUserDataDelegate::CreateUObject(this, &UActorComponent_Playfab::UpdateLoginTIme),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
	);
}

void UActorComponent_Playfab::UpdateLoginTIme(const PlayFab::ClientModels::FUpdateUserDataResult& result)
{
	UKismetMathLibrary::UtcNow();
}
////////////////////////////////////////////////