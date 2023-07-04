// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadWriteText.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

//���� �ε�
bool UReadWriteText::LoadLocalTxt(FString FileName, TArray<FString>& LoadSoundArray)
{
	//TArray<FString> OutputArray;
	FString FileContent;
	FString FilePath = FPaths::ProjectDir() + FileName;
	
	if (FFileHelper::LoadFileToString(FileContent, *FilePath))
	{
		//�Ľ�
		FileContent.ParseIntoArray(LoadSoundArray, TEXT(","), true);
		UE_LOG(LogTemp, Warning, TEXT("File loaded successfully: %s"), *FilePath);
		
		return true;
	}
	else 
	{
		// ���� �б� ����
		UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
		return false;
	}
}
//���� ����
bool UReadWriteText::SaveLocalTxt(TArray<FString> SaveSoundArray, FString FileName)
{
	// ������ �迭 Join
	FString OutPutString = FString::Join(SaveSoundArray, TEXT(","));

	// �ؽ�Ʈ ���Ϸ� ����
	if (FFileHelper::SaveStringToFile(OutPutString, *(FPaths::ProjectDir() + FileName)))
	{
		UE_LOG(LogTemp, Warning, TEXT("File Save Success: %s"), *FileName);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("File Save Fail: %s"), *FileName);
		return false;
	}
}
