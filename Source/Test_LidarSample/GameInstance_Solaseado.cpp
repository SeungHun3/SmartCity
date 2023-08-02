// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance_Solaseado.h"

#include "Kismet/KismetSystemLibrary.h"





UGameInstance_Solaseado::UGameInstance_Solaseado()
{
	// 커스텀 데이터 테이블 세팅하기
	static ConstructorHelpers::FObjectFinder<UDataTable> FindTableData_Hair(TEXT("/Game/Project/DataTable/Data_Folder/Data_Hair"));
	if (FindTableData_Hair.Succeeded())
	{
		CustomDataArray.Add(FindTableData_Hair.Object);
	}
	//CustomDataArray[0] = FindTableData.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> FindTableData_Shoes(TEXT("/Game/Project/DataTable/Data_Folder/Data_Shoes"));
	if (FindTableData_Shoes.Succeeded())
	{
		CustomDataArray.Add(FindTableData_Shoes.Object);
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> FindTableData_Top(TEXT("/Game/Project/DataTable/Data_Folder/Data_Top"));
	if (FindTableData_Top.Succeeded())
	{
		CustomDataArray.Add(FindTableData_Top.Object);
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> FindTableData_Bottoms(TEXT("/Game/Project/DataTable/Data_Folder/Data_Bottoms"));
	if (FindTableData_Bottoms.Succeeded())
	{
		CustomDataArray.Add(FindTableData_Bottoms.Object);
	}
	
	////////////////////////////

	// 보상데이터 세팅하기
	static ConstructorHelpers::FObjectFinder<UDataTable> FindCheckingReward(TEXT("/Game/Project/DataTable/Data_Folder/Reward_Folder/Data_Checking_Reward"));
	if (FindCheckingReward.Succeeded())
	{
		Checking_Reward_Table = FindCheckingReward.Object;
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> FindSpecialReward(TEXT("/Game/Project/DataTable/Data_Folder/Reward_Folder/Data_Special_Reward"));
	if (FindCheckingReward.Succeeded())
	{
		Special_Reward_Table = FindSpecialReward.Object;
	}
	// 퀘스트 데이터 세팅
	static ConstructorHelpers::FObjectFinder<UDataTable> FindQuestData(TEXT("/Game/Project/DataTable/Data_Folder/Quest_Folder/Data_Quest_Main"));
	if (FindQuestData.Succeeded())
	{
		Quest_Table = FindQuestData.Object;
	}


	//업적데이터 세팅
	static ConstructorHelpers::FObjectFinder<UDataTable> FindAchieveData(TEXT("/Game/Project/DataTable/Data_Folder/Achieve_Folder/Data_Achievement"));
	if (FindAchieveData.Succeeded())
	{
		Achieve_Table = FindAchieveData.Object;
	}

	//////////////////////////
	// Npc 대화 데이터 세팅
	static ConstructorHelpers::FObjectFinder<UDataTable> FindDialogueNpcData(TEXT("/Game/Project/DataTable/Data_Folder/DialogueNpc_Folder/DialogueNpc_Main"));
	if (FindDialogueNpcData.Succeeded())
	{
		DialogueNpc_Table = FindDialogueNpcData.Object;
	}
}

// ITemID로 Mesh, Image,,을 담는 FCustomizing_Struct 구조체 반환처리하는 함수
FCustomizing_Struct UGameInstance_Solaseado::Find_ITem(const FString& ITemID)
{

	// 아이디 첫번째 String 담아놓기
	//TCHAR FirstString = ITemID.begin().operator*(); // 첫번째 문자열 뽑기 다른방법 있으면 찾아보자
	//UE_LOG(LogTemp, Log, TEXT("// FString : %c "), FirstString);



	if (!CustomDataArray.IsValidIndex(0))
	{
		UE_LOG(LogTemp, Log, TEXT("// GameInstance =>  NoDataTable "));
		return FCustomizing_Struct();
		//FCustomizing_Struct(); 를 반환시 ItemID.IsEmpty() 함수로 데이터를 담았는지 확인가능
	}



	FCustomizing_Struct checkstruct;
	for (auto it : CustomDataArray)
	{
		const TMap<FName, uint8*> Rowmap = it->GetRowMap();
		TArray<FName> keyArray;
		Rowmap.GetKeys(keyArray);


		for (auto key : keyArray)
		{
			FString contextTemp;
			checkstruct = *it->FindRow<FCustomizing_Struct>(key, contextTemp);

			// key = 데이터테이블 행순서 int를 FString으로 변환시킨 값
			if (ITemID == checkstruct.ItemID)
			{
				//UE_LOG(LogTemp, Log, TEXT("// KeyName %s "), *ITemID);
				return checkstruct;
			}
		}
	}

	// 일치하는 아이템이 없다면 여기까지 오게됨
	UE_LOG(LogTemp, Log, TEXT("// GameInstance =>  Nodata!!!!! "));

	return FCustomizing_Struct();
}

