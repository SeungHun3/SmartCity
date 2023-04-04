// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent_Playfab.h"
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

	// ...
}


// Called when the game starts
void UActorComponent_Playfab::BeginPlay()
{
	Super::BeginPlay();

	// ...C9B19 Playfab title ID
	GetMutableDefault<UPlayFabRuntimeSettings>()->TitleId = TEXT("C9B19");
	
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
	// Ŀ���� ���̵�
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
			UE_LOG(LogTemp, Log, TEXT("// SessionTicket :: ( %s )"), *result.AuthenticationContext->GetClientSessionTicket());

			// ���� ���� ���� üũ
			// getServerTitleData(result.NewlyCreated);
			SuccessLogin(result.NewlyCreated);

			}),
		FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// Email Login 
void UActorComponent_Playfab::Login_Email(const FString& email, const FString& pw)
{
	// ���� ���̵� ó�� 
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	ClientModels::FLoginWithEmailAddressRequest request;

	request.Email = email;
	request.Password = pw;

	ClientAPI->LoginWithEmailAddress(
		request,
		UPlayFabClientAPI::FLoginWithEmailAddressDelegate::CreateLambda([&](const ClientModels::FLoginResult& result) {

			PlayFabID = result.PlayFabId;
			LastLoginTime = result.LastLoginTime.mValue.ToString();
			UE_LOG(LogTemp, Log, TEXT("// EmailAddress Login Success :: %s ( %s )"), *PlayFabID, *LastLoginTime);
			UE_LOG(LogTemp, Log, TEXT("// SessionTicket :: ( %s )"), *result.AuthenticationContext->GetClientSessionTicket());
			// getUserTitleData();

			}),
		FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}

// Playfab Script
// �÷����� ��ũ��Ʈ �̺�Ʈ ȣ��
// // �Ű����� x 
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
// // �Ű����� o
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
// // �Ű����� �迭 
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

// ��ũ��Ʈ ȣ�� ���� �̺�Ʈ 
// Playfab �ܺ� ��ũ��Ʈ ȣ��� ���� ��� ������ ������ ��
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

// ��ũ��Ʈ ȣ�� ����
void UActorComponent_Playfab::ErrorScript(const PlayFab::FPlayFabCppError& error)
{
	FString str = error.GenerateErrorReport();

	UE_LOG(LogTemp, Log, TEXT("// Error Script ( %s ):: %s"), *error.ErrorName, *str);

	// "Item not found" ������ ���Ž� �߸��� ���� �Է�
	if (error.ErrorName == "Item not found")
	{
		UE_LOG(LogTemp, Log, TEXT("// Item not found"));
		return;
	}

	//// �ߺ� �̸� , �̸� ���� ���� ���� (string 3 ~24)
	//if (error.ErrorName == "NameNotAvailable" || "InvalidParams")
	//{
	//	CheckerCharacterName = error.ErrorName;
	//	bCheckerCreateChracter = false;
	//	CustomizingWidgetNameChecker(false);
	//}
	//// ������ ���� ����
	//// "WrongPrice" || "WrongVirtualCurrency") // ItemNotFound , StoreNotFound
	//if (error.ErrorName == "InsufficientFunds")
	//{
	//	// �� ����
	//	AddPopupWidget(nullptr, enum_PopupStyle::Popup1Key, enum_PopupRun::Purchage,
	//		FText::FromString(" You cannot purchase the item because you don't have enough money."), FText::FromString("Purchase failed"));
	//}
	//// ���̵� ��
	//if (error.ErrorName == "AccountBanned")
	//{
	//	// ���� , ���� ����
	//	// FString EmaliCheck;
	//	AddPopupWidget(nullptr, enum_PopupStyle::Popup1Key, enum_PopupRun::Cancel,
	//		FText::FromString(str + "If you want to check the Ban information, contact ad@ma.com."), FText::FromString("AccountBanned"));
	//}
}

// Playfab Response
// �÷����� ��ũ��Ʈ �ݺ� �̺�Ʈ
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

	// �÷��̾� ������ ������ ������Ʈ
	if (Selection == "updateDefaultValue")
	{
		UE_LOG(LogTemp, Log, TEXT("// Playfab _ updateDefaultValue :: %s"), *getStringData);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return Callback Event 

void UActorComponent_Playfab::SuccessLogin(bool NewlyCreated)
{
	// ù ���� ���ο� ���� ����Ʈ ������ �߰� 
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

	// ã���� �ϴ� Ÿ��Ʋ // ������ = ��ü Ÿ��Ʋ ������
	TArray<FString> targetKey;
	targetKey.Add(targetTitle);

	request.Keys = targetKey;
	request.PlayFabId = PlayFabID;
	// ���� ������ ã��
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
					// ģ�е�
					if (it == FString("Fellowship"))
					{
						// Fellowship = record->Value;
						UE_LOG(LogTemp, Log, TEXT("// GetUserData() Fellowship :: %s "), *record->Value);
					}
					// ���� ��Ų �÷�
					else if (it == FString("Skincolor"))
					{
						// CustomData.Skin = *record->Value;
						// setPlayerSkinCostume(*record->Value);
					}
					// ĳ���� ���� ���� üũ
					else if (it == FString("Createcharacter"))
					{
						if (*record->Value == FString("1"))
						{
							bCheckerCreateChracter = true;
							UE_LOG(LogTemp, Log, TEXT("// Checker Create Chracter :: %d "), true);
							// ĳ���� �÷����� ������ ��������.
							// �κ��丮 > 
							
						}
						else
						{
							bCheckerCreateChracter = false;
							UE_LOG(LogTemp, Log, TEXT("// Checker Create Chracter :: %d "), false);
							// ĳ���� ���� �ϱ�
							// createPlayfabCharacterEvent();
						}
						// ĳ���� ���� �ʱ� ������ 
						// InitPlayFabUserTitleData(bCheckerCreateChracter);
					}
					else
						UE_LOG(LogTemp, Log, TEXT("// GetUserData() Key ( %s ) , Value ( %s ) "), *it, *record->Value);
				}
			}
			// UserCharacterName // Playfab Display Name üũ
			getUserTitleName();
			// ���� ������ üũ
			// getStatisticsEvent();
			// ���� ���� üũ
			// getNoticeEvent();

			// updateUserTitleData(UserTitleData);
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// ���� �г���
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

			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// ������ �κ��丮 
void UActorComponent_Playfab::getInventoryList()
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	PlayFab::ClientModels::FGetUserInventoryRequest request;

	ClientAPI->GetUserInventory(
		request,
		PlayFab::UPlayFabClientAPI::FGetUserInventoryDelegate::CreateLambda([&](const PlayFab::ClientModels::FGetUserInventoryResult& result) {

			//InventoryProperty.Reset();
			UE_LOG(LogTemp, Log, TEXT("// getInventoryList :: Len ( %d )"), result.Inventory.Num());

			//for (auto it : result.Inventory)
			//{
			//	FItemproperty Invendata;
			//	Invendata.ItemId = it.ItemId;
			//	Invendata.ItemInstanceId = it.ItemInstanceId;
			//	Invendata.ItemClass = it.ItemClass;
			//	Invendata.RemainingUses = it.RemainingUses;
			//	Invendata.UnitPrice = it.UnitPrice;
			//	if (it.CustomData.Contains("Color"))
			//	{
			//		FString* colorData = it.CustomData.Find("Color");
			//		Invendata.colorData = colorData[0];
			//		UE_LOG(LogTemp, Log, TEXT("// getInventoryList :: Color ( %s )"), *Invendata.colorData);
			//	}
			//	else
			//		UE_LOG(LogTemp, Log, TEXT("// Error : getInventoryList(id : %s ) Not Find ColorData"), *it.ItemId);

			//	if (CheckUsertitleData("Body", Invendata)) {

			//		TTuple<FName, FName> ChangeBody(FName(*Invendata.ItemId), FName(*Invendata.colorData));
			//		CustomData.Body = ChangeBody;
			//	}
			//	else if (CheckUsertitleData("Eyebrow", Invendata)) {
			//		TTuple<FName, FName> ChangeBody(FName(*Invendata.ItemId), FName(*Invendata.colorData));
			//		CustomData.Eyebrow = ChangeBody;
			//	}
			//	else if (CheckUsertitleData("Eyes", Invendata)) {
			//		TTuple<FName, FName> ChangeBody(FName(*Invendata.ItemId), FName(*Invendata.colorData));
			//		CustomData.Eyes = ChangeBody;
			//	}
			//	else if (CheckUsertitleData("Hair", Invendata)) {
			//		TTuple<FName, FName> ChangeBody(FName(*Invendata.ItemId), FName(*Invendata.colorData));
			//		CustomData.Hair = ChangeBody;
			//	}
			//	else if (CheckUsertitleData("Lips", Invendata)) {
			//		TTuple<FName, FName> ChangeBody(FName(*Invendata.ItemId), FName(*Invendata.colorData));
			//		CustomData.Lips = ChangeBody;
			//	}
			//	// �κ��丮 ������ ���� ����Ʈ
			//	InventoryProperty.Add(Invendata);
			//	// �κ��丮 ���� �߰� 
			//	updateInventoryItem(Invendata);
			//}

			if (result.VirtualCurrency.Contains("GC"))
			{
				const int* findVC = result.VirtualCurrency.Find("GC");
				// updateInventoryCoin(FString::FromInt(findVC[0]));
			}

			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}