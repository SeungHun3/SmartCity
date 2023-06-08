// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance_Solaseado.h"

#include "Kismet/KismetSystemLibrary.h"





UGameInstance_Solaseado::UGameInstance_Solaseado()
{
	// ������ ���̺� �����ϱ�
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
	// ���� ��

}




// ITemID�� Mesh, Image,,�� ��� FCustomizing_Struct ����ü ��ȯó���ϴ� �Լ�
FCustomizing_Struct UGameInstance_Solaseado::Find_ITem(const FString& ITemID)
{

	// ���̵� ù��° String ��Ƴ���
	//TCHAR FirstString = ITemID.begin().operator*(); // ù��° ���ڿ� �̱� �ٸ���� ������ ã�ƺ���
	//UE_LOG(LogTemp, Log, TEXT("// FString : %c "), FirstString);



	if (!CustomDataArray.IsValidIndex(0))
	{
		UE_LOG(LogTemp, Log, TEXT("// GameInstance =>  NoDataTable "));
		return FCustomizing_Struct();
		//FCustomizing_Struct(); �� ��ȯ�� ItemID.IsEmpty() �Լ��� �����͸� ��Ҵ��� Ȯ�ΰ���
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

			// key = ���������̺� ����� int�� FString���� ��ȯ��Ų ��
			if (ITemID == checkstruct.ItemID)
			{
				//UE_LOG(LogTemp, Log, TEXT("// KeyName %s "), *ITemID);
				return checkstruct;
			}
		}
	}

	// ��ġ�ϴ� �������� ���ٸ� ������� ���Ե�
	UE_LOG(LogTemp, Log, TEXT("// GameInstance =>  Nodata!!!!! "));

	return FCustomizing_Struct();
}

