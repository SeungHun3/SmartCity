// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadWriteText.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

//���� �ε�
bool UReadWriteText::LoadLocalTxt(FString FileName, TArray<FString>& LoadSoundArray, TArray<FString>& LoadMuteArray)
{
	//TArray<FString> OutputArray;
	FString FileContent;
	FString FilePath = FPaths::ProjectDir() + FileName;
	// ���� ������ �迭 ������ ����
	TArray<FString> SaveTypeArray;

	if (FFileHelper::LoadFileToString(FileContent, *FilePath))
	{
		//�Ľ�
		FileContent.ParseIntoArray(SaveTypeArray, TEXT("\n"), true);
		
		if (SaveTypeArray.Num() >= 2)
		{
			SaveTypeArray[0].ParseIntoArray(LoadSoundArray, TEXT(","), true); //Sound
			SaveTypeArray[1].ParseIntoArray(LoadMuteArray, TEXT(","), true); //Mute
		}
		// �迭 ���� üũ, �ʱ�ȭ
		if (LoadSoundArray.Num() != 4)
		{
			LoadSoundArray.Init("5", 4);
		}

		if (LoadMuteArray.Num() != 4)
		{
			LoadMuteArray.Init("0", 4);
		}

		return true;
	}
	else 
	{
		// ���� �б� ����
		return false;
	}
}
//���� ����
bool UReadWriteText::SaveLocalTxt(TArray<FString> SaveSoundArray, TArray<FString> SaveMuteArray, FString FileName)
{
	// ���� ������ �迭 ������ ����
	TArray<FString> SaveTypeArray;
	// ������ �迭 Join
	SaveTypeArray.Add(FString::Join(SaveSoundArray, TEXT(","))); //Sound
	SaveTypeArray.Add(FString::Join(SaveMuteArray, TEXT(","))); //Mute
	
	// �迭 ���� üũ, �ʱ�ȭ
	if (SaveSoundArray.Num() != 4)
	{
		SaveSoundArray.Init("5", 4);
	}

	if (SaveMuteArray.Num() != 4)
	{
		SaveMuteArray.Init("0", 4);
	}

	//�� Ÿ���� ���� ���ڷ� Join
	FString OutPutString = FString::Join(SaveTypeArray, TEXT("\n"));

	// �ؽ�Ʈ ���Ϸ� ����
	if (FFileHelper::SaveStringToFile(OutPutString, *(FPaths::ProjectDir() + FileName)))
	{
		return true;
	}
	else
	{
		return false;
	}
}
