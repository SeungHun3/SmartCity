// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent_Playfab.h"
#include "Pawn_Player.h"
#include "Engine/Engine.h"
#include "Widget/Checking_Folder/Widget_CheckingAttandance_Main.h"
#include "GameInstance_Solaseado.h"

#include "Core/PlayFabClientAPI.h"

#include "PlayFabJsonObject.h"
#include "PlayFabJsonValue.h"
#include "Kismet/DataTableFunctionLibrary.h"


using namespace PlayFab;

// Sets default values for this component's properties
UActorComponent_Playfab::UActorComponent_Playfab()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

		// ��Ģ�� ����Ʈ > �г���
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
			// UE_LOG(LogTemp, Log, TEXT("// SessionTicket :: ( %s )"), *result.AuthenticationContext->GetClientSessionTicket());

			// ���� ���� ���� üũ
			// getServerTitleData(result.NewlyCreated);

			// �α��� ���� // NewlyCreated �ֱ� ���� ���� �Ǵ�
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
	// �ߺ� �̸� , �̸� ���� ���� ���� (string 3 ~24)
	if (error.ErrorName == "NameNotAvailable" || "InvalidParams")
	{
		// CheckerCharacterName = error.ErrorName;
		bCheckerCreateChracter = false;
		// ���н� ���� ������ ���� 
		if (PlayerOwner)
			PlayerOwner->CustomizingWidgetNameChecker(bCheckerCreateChracter);
	}
	//// ���̵� ��
	//if (error.ErrorName == "AccountBanned")
	//{
	//	// ���� , ���� ����
	//	// FString EmaliCheck;
	//	AddPopupWidget(nullptr, enum_PopupStyle::Popup1Key, enum_PopupRun::Cancel,
	//		FText::FromString(str + "If you want to check the Ban information, contact ad@ma.com."), FText::FromString("AccountBanned"));
	//}
}

//////////////////////////////////////////////////////
// �⼮üũ 
// ���� üũ�ߴ��� Ȯ�ο���
bool UActorComponent_Playfab::Is_Today_Checked()
{
	if (PlayFab_Statistics.Contains("Is_Checked_Daily")) // �÷����� �������忡�� ���� üũ�� �ߴ��� Ȯ�� �Ŀ�
	{
		int32 isChecked = *PlayFab_Statistics.Find("Is_Checked_Daily");
		// ���� üũ�� �ߴٸ� = true, ���ߴٸ� false
		if (isChecked == 1)
		{
			return true;
		}
		else { return false; }
		
	}
	else //Is_Checked_Daily�� Ű�� ���ٸ�
	{
		ScriptCustomNotField("Default_Statistics");
		UE_LOG(LogTemp, Log, TEXT("// NoData : Is_Checked_Daily"));
		return false;

	}
}
// ���ñ��� üũ�� �⼮�ϼ�
int UActorComponent_Playfab::Get_Checking_Count()
{
	if (PlayFab_Statistics.Contains("Month_Reward_Count")) // �⼮ ī��Ʈ ������ �����ͼ� ��ȯ
	{
		return *PlayFab_Statistics.Find("Month_Reward_Count");
	}
	else // �⼮ �����Ͱ� ���ٸ� 
	{
		ScriptCustomNotField("Default_Statistics");
		UE_LOG(LogTemp, Log, TEXT("// NoData : Month_Reward_Count"));
		return 0;
	}
}

//PlayFab���� daily check ������Ʈ + checkCount ++ -> return checkCount 
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
				result_Object->Values.GetKeys(Keys); // ���ϰ� �ϳ��� Key �� ������


				if ((Keys[0] == "Month_Reward_Count")&& result_Object->Values.Contains(Keys[0]))
				{
						int CheckingCount = static_cast<int>(result_Object->Values.Find(Keys[0])->Get()->AsNumber()); // ���Ϲ��� Count��
						
						if (PlayFab_Statistics.Contains(Keys[0]) && PlayFab_Statistics.Find(Keys[0]))
						{
							PlayFab_Statistics.Add(Keys[0], CheckingCount); // Map ������ �����Ű��
							// ������ ����
							Widget->ChangeSlot();
						}
				}
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// 20�� ����
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
				//������ �����ְ��� �����Լ� ȣ��
				Widget->ChangeSpecial();
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);

}


// ������ �⼮üũ �ʱ�ȭ
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

				//������ �����ְ��� �����Լ� ȣ��
				Widget->Debug_Finished_Clear();
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// ������ ���� �⼮���� �ʱ�ȭ
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

				//������ �����ְ��� �����Լ� ȣ��
				Widget->Debug_Finished_DailyClear();
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}


//////////////////////////////////////////////////////
// �⼮üũ ��




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
	if (Selection == "UpdataDefaultValue")
	{
		UE_LOG(LogTemp, Log, TEXT("// Playfab _ updateDefaultValue :: %s"), *getStringData);
		getUserTitleData();
	}
	else if (Selection == "Createcharacter")
	{
		UE_LOG(LogTemp, Log, TEXT("// Playfab _ Createcharacter :: %s"), *getStringData);
		// Test _ ĳ���� ���� �Ϸ�� ���� ���� ���� ó�� 
		if (PlayerOwner)
			PlayerOwner->Blueprint_CreateCharacter();
	}
	else if (Selection == "Create_Grant")
	{
		UE_LOG(LogTemp, Log, TEXT("// Playfab _ GrantSuccess__call__getUserTitleData"));
		// �������� �������� �ְ� �κ��丮�� �ְ� Ÿ��Ʋ �����ͷ� ���� -> Ÿ��Ʋ ������ ���������� �����̵�����
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

		//������ ��� ���� �̺�Ʈ.
		UE_LOG(LogTemp, Log, TEXT("// Playfab _ updateEquipmentItem :: %s"), *getStringData);
		if (PlayerOwner)
			PlayerOwner->Blueprint_UpdateEquipmentItem(getStringData);
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
					// ĳ���� ���� ���� üũ
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
					// ģ�е�
					else if (it == FString("Fellowship"))
					{
						// Fellowship = record->Value;
						UE_LOG(LogTemp, Log, TEXT("// GetUserData() Fellowship :: %s "), *record->Value);
					}
					else
						UE_LOG(LogTemp, Log, TEXT("// GetUserData() Key ( %s ) , Value ( %s ) "), *it, *record->Value);
				}
			}
			// ĳ���� ���� ����
			if (PlayerOwner)
				PlayerOwner->InitPlayFabUserTitleData(bCheckerCreateChracter);

			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// �ΰ��� ���ӽ� �÷��̾� ������ �ε� 
// �ε� ���� �߰� �ϱ�. �׽�Ʈ ���� ������ �ӽ� �߰�
// PlayerOwner->Check_getIngameLoadingCount()
void UActorComponent_Playfab::getIngamePlayerData()
{
	// Check_getIngameLoadingCount :: ���� ���� �����͸� �ҷ����� �Լ� ���� üũ
	// Test Count :: 1
	if (PlayerOwner)
		PlayerOwner->Test_LoadingCount = 1;

	// Test_LoadingCount Check :: o
	// UserCharacterName // Playfab Display Name üũ
	getUserTitleName();

	// Test_LoadingCount Check :: x
	// �κ��丮 ����
	getInventoryList();
	// ���� ������ üũ
	getStatisticsEvent();
	// ����Ʈ ������ üũ
	GetMyQuestToServer();
	// ���� ���� üũ
	getNoticeEvent(3);
	// updateUserTitleData(UserTitleData);
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
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("// getAccountInfo titleName :: %s "), *UserCharacterName));
			}
			if (PlayerOwner)
				PlayerOwner->Check_getIngameLoadingCount();
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

				// ������ �÷� ���� ���� ��� ������ �ʿ�.
				// ������ ���� ������ ���̺�� ������ ó�� ���? 
				/*if (it.CustomData.Contains("Color"))
				{
					FString* colorData = it.CustomData.Find("Color");
					Invendata.colorData = colorData[0];
					UE_LOG(LogTemp, Log, TEXT("// getInventoryList :: Color ( %s )"), *Invendata.colorData);
				}
				else
					UE_LOG(LogTemp, Log, TEXT("// Error : getInventoryList(id : %s ) Not Find ColorData"), *it.ItemId);*/

				// Ŀ���͸����� ���� ������ ���� ����.
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
				// �κ��丮 ������ ���� ����Ʈ
				InventoryProperty.Add(Invendata);

				// �κ��丮 ���� ���� �߰� �Լ� ����
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

			// �ε� ī���� üũ �� ���� ���� ���� ���� Ȯ��. // �ε� ī���� ���� 
			/*if (PlayerOwner)
				PlayerOwner->Check_getIngameLoadingCount();*/
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}
// �س��� ��Ģ�� üũ�ϱ�
bool UActorComponent_Playfab::FindCheckBadName(const FString& name)
{
	TArray<FName> outputBadNameCheckRow = BadNameTable->GetRowNames();

	// �г��� ���� ���鹮�� x
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
// ���� �г��� ������Ʈ (�ߺ� üũ, ����)
void UActorComponent_Playfab::updateUserTitleName(const FString& DisplayName)
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	PlayFab::ClientModels::FUpdateUserTitleDisplayNameRequest request;
	request.DisplayName = DisplayName;

	if (FindCheckBadName(DisplayName))
	{
		// ��Ģ�� ó�� �߰� �ϱ�.
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
// ���� ������ getStatisticsEvent
// �̼� ������ & ���(��ŷ) ������ 
void UActorComponent_Playfab::getStatisticsEvent()
{
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	PlayFab::ClientModels::FGetPlayerStatisticsRequest request;
	
	ClientAPI->GetPlayerStatistics(
		request,
		PlayFab::UPlayFabClientAPI::FGetPlayerStatisticsDelegate::CreateLambda([&](const PlayFab::ClientModels::FGetPlayerStatisticsResult& result) {
			// ������ ��赥���� ä���ֱ�
			for (auto it : result.Statistics)
			{
				PlayFab_Statistics.Add(it.StatisticName, it.Value);
			}
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
		);
}

// ���� ���� �������� NoticeCount ������ ���� ��
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
				// ���� �˾��� ���� ���� ó��
				// ���� // ����
				// updateNotice(it.Title, it.Body);
				UE_LOG(LogTemp, Log, TEXT("// getNotice Body :: %s "), *it.Body);
			}
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}

/////////////////////////////////Grant -> getinGamePlayerData -> level�̵�
// FunctionName : Create_Grant  == �ڽ�Ƭ �÷����տ� ���ε� 
void UActorComponent_Playfab::UploadMyCustom(const FString& FunctionName, const FString& FieldName,const TArray<int> ItemIDs)
{
	TArray<FString> FirstCostumeData;

	for (auto it : ItemIDs)
	{
		FirstCostumeData.Push(FString::FromInt(it));
	}
	// Ŀ���� ������ �ϴ� ����
	ScriptCustomArray(FunctionName, FieldName, FirstCostumeData);
}

void UActorComponent_Playfab::GetMyQuestToServer()
{	
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	PlayFab::ClientModels::FGetUserDataRequest request;
	FString targetKey = "Quest";
	request.Keys.Add(targetKey);
	request.PlayFabId = PlayFabID;


	// ����Ʈ ������ ã��
	ClientAPI->GetUserData(
		request,
		PlayFab::UPlayFabClientAPI::FGetUserDataDelegate::CreateLambda([&, targetKey](const PlayFab::ClientModels::FGetUserDataResult& result) {

			const PlayFab::ClientModels::FUserDataRecord* record = result.Data.Find(targetKey);
			if (!record)
			{
				Change_Quest_Main_or_Step(true);
				return;
			}
			// ���� �����͸� �����ͼ� ������ ����
			TArray<FString> QuestKeys;
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(record->Value); // json������ FString �� jsonReader�� ���� ������ ����
			//������Ʈ�� ���ٸ� == ����Ʈ �����Ͱ� ���ٸ� (�ʱ� Init������ 0���� ����) ==> ����Ʈ ������ �����Ű��
			if (!FJsonSerializer::Deserialize(JsonReader, JsonObject)) // ���� �����͸� jsonObject�� �ֱ�
			{
				Change_Quest_Main_or_Step(true);
				return;
			}
			
			//MyQuestInfo �� Main, Step �ֱ� 
			JsonObject->Values.GetKeys(QuestKeys);
			if (QuestKeys.IsValidIndex(0))
			{
				TArray<FString> ParseData;
				QuestKeys[0].ParseIntoArray(ParseData, TEXT("/n"));
				if (!ParseData[0].IsEmpty()) { MyQuestInfo.Quest_Name = FCString::Atoi(*ParseData[0]); }
				if (!ParseData[1].IsEmpty()) { MyQuestInfo.Quest_Step = FCString::Atoi(*ParseData[1]); }
			}

			//MyQuestInfo �� bool�־��ֱ�
			for (auto it : QuestKeys)
			{
				if (JsonObject->Values.Contains(it))
				{
					MyQuestInfo.indexCheck.Add(JsonObject->Values.Find(it)->Get()->AsBool());
				}
			}
			
			Setting_QuestData();							//����� ������ ������ ���̺� �ҷ�����
			if (!MyQuestInfo.QuestTable) 	{	return;	}	//���̺��� ���ٸ� �������Ʈ �Ϸ��߰ų� ���̺� ����
			Play_Quest(MyQuestInfo.QuestTable,MyQuestInfo.Quest_Step);
			}));
	
				
}

void UActorComponent_Playfab::UpdateMyQuest()
{
	if (!MyQuestInfo.indexCheck.IsValidIndex(0)) {		return;	}

	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	TSharedPtr<FJsonObject> JsonSubObj = MakeShareable(new FJsonObject());

	if (!JsonObj.IsValid() && !JsonSubObj.IsValid())	{	UE_LOG(LogTemp, Log, TEXT("// JSonObj isValid "));		return;	}

	//����ü ������ ��Ƽ� 
	FString ParseKey;
	ParseKey.Append(FString::FromInt(MyQuestInfo.Quest_Name));
	ParseKey.Append("/n");
	ParseKey.Append(FString::FromInt(MyQuestInfo.Quest_Step));
	ParseKey.Append("/n");

	for (int i = 0; i< MyQuestInfo.indexCheck.Num(); i++)
	{
		FString QuestKey = ParseKey + FString::FromInt(i);
		JsonSubObj->SetBoolField(QuestKey, MyQuestInfo.indexCheck[i]);
	}
	JsonObj->SetObjectField("Quest", JsonSubObj);

	//������ ����
	PlayFab::ClientModels::FExecuteCloudScriptRequest request;
	request.FunctionName = "UpdateQuest";
	request.FunctionParameter = JsonObj;
	request.GeneratePlayStreamEvent = true;
	ClientAPI->ExecuteCloudScript(
		request,
		UPlayFabClientAPI::FExecuteCloudScriptDelegate::CreateLambda([&](const PlayFab::ClientModels::FExecuteCloudScriptResult& result)
			{
				PlayerOwner->Quest_Update();
			}),
		PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UActorComponent_Playfab::ErrorScript)
				);
}

TArray<int> UActorComponent_Playfab::GetQuestRowNames(const FString& QuestStep, UDataTable* QuestTable)
{
	TArray<FString> RowNames;
	TArray<int> QuestRowNames;
	RowNames = UDataTableFunctionLibrary::GetDataTableColumnAsString(QuestTable, FName("QuestName"));

	for (int i = 0; i < RowNames.Num(); i++)
	{
		if (RowNames[i] == QuestStep)
		{
			QuestRowNames.Add(i + 1);
		}
	}
	return QuestRowNames;
}

void UActorComponent_Playfab::Setting_QuestData()
{
	MyQuestInfo.QuestTable = nullptr;
	
	UGameInstance_Solaseado* MyInstance = Cast<UGameInstance_Solaseado>(GetWorld()->GetGameInstance());
	UDataTable* Quest_Table = MyInstance->GetQuestTables();
	FQuest_List* QuestList = Quest_Table->FindRow<FQuest_List>(FName(*FString::FromInt(MyQuestInfo.Quest_Name)), "MainQuest_Not_Found_cpp870");
	//���� �´� DataTable ã�Ƽ� ���� //QuestList ������ ��������Ʈ �Ϸ� // MyQuestInfo.QuestTable �� ������ ��������Ʈ�� DataTable����
	if (QuestList && QuestList->QuestData)
	{
		MyQuestInfo.QuestTable = QuestList->QuestData;
	}
}

bool UActorComponent_Playfab::Play_Quest(UDataTable* QuestData, int Step)
{
	bool isChangeStep = false;

	TArray<int> rowNames = GetQuestRowNames(FString::FromInt(Step), QuestData);

	if (!rowNames.IsValidIndex(0))
	{
		Change_Quest_Main_or_Step(true);
		return isChangeStep;
	}
	
	//����Ʈ Play
	for (int i = 0; i < rowNames.Num(); i++)
	{
		if (MyQuestInfo.indexCheck.IsValidIndex(i)) // �̹� �����͸� ������ �ִ°��(change���� clear�ȵ� ������ == getServer���� �ٷ� Play�� ��� )
		{
			if (MyQuestInfo.indexCheck[i]) {continue; } //�������� �Ϸ��� ����Ʈ�� �������
		}
		else // �����Ͱ� ���°�� ex) change, init character
		{
			MyQuestInfo.indexCheck.Add(false);
		}
		
		FQuest_List QuestContents = *QuestData->FindRow<FQuest_List>(FName(*FString::FromInt(rowNames[i])), ""); // ����Ʈ ����
		PlayerOwner->Quest_Binding(MyQuestInfo,i, QuestContents);
	}
	PlayerOwner->Quest_Play();

	isChangeStep = true;
	return isChangeStep;
}

void UActorComponent_Playfab::Change_Quest_Main_or_Step(bool isMain)
{
	MyQuestInfo.indexCheck.Reset();
	
	if (isMain) // MainQuest�� �����ٸ�
	{
		MyQuestInfo.Quest_Name += 1;
		MyQuestInfo.Quest_Step = 1;
		Setting_QuestData(); // DataTable ä���
	}
	else { MyQuestInfo.Quest_Step += 1;	} // Step�� �����ٸ�

	if (!MyQuestInfo.QuestTable)// ���̺��� ���ٸ�
	{
		MyQuestInfo.Quest_Step = 1;
		PlayerOwner->Quest_All_Finished();
		return;
	}

	//Change ������ ������ ������Ʈ�ϱ�
	if (Play_Quest(MyQuestInfo.QuestTable, MyQuestInfo.Quest_Step))	{		UpdateMyQuest();	}
	
}

void UActorComponent_Playfab::Change_QuestFinished(int index)
{
	if (MyQuestInfo.indexCheck.IsValidIndex(index))
	{
		MyQuestInfo.indexCheck[index] = true;
	}
	UpdateMyQuest();
}

//���� ������ ������ ������Ʈ�ϱ�
void UActorComponent_Playfab::UpdateAchievement(int AchieveNumber)
{
	FString Temp;
	FString AchieveCount = FString::FromInt(AchieveNumber); // AchieveNumber = 001~999 Naming ���߱��
	for (int i = 0; i < 3 - AchieveCount.Len(); i++)
	{
		Temp += "0";
	}
	AchieveCount = Temp + AchieveCount;
	
	FString FullName = AchieveName + AchieveCount;
	PlayFabClientPtr ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject());
	JsonObj->SetStringField("achieve", FullName);
	PlayFab::ClientModels::FExecuteCloudScriptRequest request;
	request.FunctionName = "Update_Achievement";
	request.FunctionParameter = JsonObj;
	request.GeneratePlayStreamEvent = true;
	ClientAPI->ExecuteCloudScript(
		request,
		UPlayFabClientAPI::FExecuteCloudScriptDelegate::CreateLambda([&, FullName, AchieveCount](const PlayFab::ClientModels::FExecuteCloudScriptResult& result)
			{
				//����������Ʈ ������ ȣ��
				PlayFab_Statistics.Add(FullName, 0);
				PlayerOwner->Finished_UpdateAchieve(AchieveCount);
			}));
			
}
//���������͸� �߷��� ��ȯ
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
		// ����������� Ŭ�����ϴ� ��� Ȯ���غ���
	}
}
