// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance_Solaseado.h"

UGameInstance_Solaseado::UGameInstance_Solaseado()
{

	static ConstructorHelpers::FObjectFinder<UDataTable> FindTableData_Hair(TEXT("/Game/Project/DataTable/Data_Folder/Data_Hair"));
	if (FindTableData_Hair.Succeeded())
	{
		CustomDataArray.Add(FindTableData_Hair.Object);
	}
	//CustomDataArray[0] = FindTableData.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> FindTableData_Hand(TEXT("/Game/Project/DataTable/Data_Folder/Data_Hand"));
	if (FindTableData_Hand.Succeeded())
	{
		CustomDataArray.Add(FindTableData_Hand.Object);
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> FindTableData_Face(TEXT("/Game/Project/DataTable/Data_Folder/Data_Face"));
	if (FindTableData_Face.Succeeded())
	{
		CustomDataArray.Add(FindTableData_Face.Object);
	}
		static ConstructorHelpers::FObjectFinder<UDataTable> FindTableData_Body(TEXT("/Game/Project/DataTable/Data_Folder/Data_Body"));
	if (FindTableData_Body.Succeeded())
	{
		CustomDataArray.Add(FindTableData_Body.Object);
	}
}
FCustomizing_Struct UGameInstance_Solaseado::Find_ITem(const FString& ITemID)
{


	TCHAR FirstString = ITemID.begin().operator*();
	UE_LOG(LogTemp, Log, TEXT("// FString : %c "), FirstString);



	if (!CustomDataArray.IsValidIndex(0))
	{
		UE_LOG(LogTemp, Log, TEXT("// GameInstance =>  NoDataTable "));
		return FCustomizing_Struct();
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

			// key는 행이름을 숫자로 만들었다
			if (ITemID == checkstruct.ItemID)
			{

				//UE_LOG(LogTemp, Log, TEXT("// KeyName %s "), *ITemID);
				return checkstruct;
			}



		}


	}
	UE_LOG(LogTemp, Log, TEXT("// GameInstance =>  Nodata!!!!! "));

	return FCustomizing_Struct();
}
