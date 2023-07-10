// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadWriteText.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

//파일 로드
bool UReadWriteText::LoadLocalTxt(FString FileName, TArray<FString>& LoadSoundArray, TArray<FString>& LoadMuteArray)
{
	//TArray<FString> OutputArray;
	FString FileContent;
	FString FilePath = FPaths::ProjectDir() + FileName;
	// 여러 종류의 배열 저장을 위해
	TArray<FString> SaveTypeArray;

	if (FFileHelper::LoadFileToString(FileContent, *FilePath))
	{
		//파싱
		FileContent.ParseIntoArray(SaveTypeArray, TEXT("\n"), true);
		
		if (SaveTypeArray.Num() >= 2)
		{
			SaveTypeArray[0].ParseIntoArray(LoadSoundArray, TEXT(","), true); //Sound
			SaveTypeArray[1].ParseIntoArray(LoadMuteArray, TEXT(","), true); //Mute
		}
		// 배열 길이 체크, 초기화
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
		// 파일 읽기 실패
		return false;
	}
}
//파일 저장
bool UReadWriteText::SaveLocalTxt(TArray<FString> SaveSoundArray, TArray<FString> SaveMuteArray, FString FileName)
{
	// 여러 종류의 배열 저장을 위해
	TArray<FString> SaveTypeArray;
	// 가져온 배열 Join
	SaveTypeArray.Add(FString::Join(SaveSoundArray, TEXT(","))); //Sound
	SaveTypeArray.Add(FString::Join(SaveMuteArray, TEXT(","))); //Mute
	
	// 배열 길이 체크, 초기화
	if (SaveSoundArray.Num() != 4)
	{
		SaveSoundArray.Init("5", 4);
	}

	if (SaveMuteArray.Num() != 4)
	{
		SaveMuteArray.Init("0", 4);
	}

	//각 타입은 개행 문자로 Join
	FString OutPutString = FString::Join(SaveTypeArray, TEXT("\n"));

	// 텍스트 파일로 저장
	if (FFileHelper::SaveStringToFile(OutPutString, *(FPaths::ProjectDir() + FileName)))
	{
		return true;
	}
	else
	{
		return false;
	}
}
