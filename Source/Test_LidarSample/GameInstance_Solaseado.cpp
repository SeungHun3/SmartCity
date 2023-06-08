// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance_Solaseado.h"

#include "Kismet/KismetSystemLibrary.h"





UGameInstance_Solaseado::UGameInstance_Solaseado()
{
	// 데이터 테이블 세팅하기
	static ConstructorHelpers::FObjectFinder<UDataTable> FindTableData_Hair(TEXT("/Game/Project/DataTable/Data_Folder/Data_Hair"));
	if (FindTableData_Hair.Succeeded())
	{
		CustomDataArray.Add(FindTableData_Hair.Object);
	}
	//CustomDataArray[0] = FindTableData.Object;
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
		static ConstructorHelpers::FObjectFinder<UDataTable> FindTableData_Shoes(TEXT("/Game/Project/DataTable/Data_Folder/Data_Shoes"));
	if (FindTableData_Shoes.Succeeded())
	{
		CustomDataArray.Add(FindTableData_Shoes.Object);
	}
	// 세팅 끝

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

