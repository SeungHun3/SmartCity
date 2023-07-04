// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadWriteText.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

//파일 로드
bool UReadWriteText::LoadLocalTxt(FString FileName, TArray<FString>& LoadSoundArray)
{
	//TArray<FString> OutputArray;
	FString FileContent;
	FString FilePath = FPaths::ProjectDir() + FileName;
	
	if (FFileHelper::LoadFileToString(FileContent, *FilePath))
	{
		//파싱
		FileContent.ParseIntoArray(LoadSoundArray, TEXT(","), true);
		UE_LOG(LogTemp, Warning, TEXT("File loaded successfully: %s"), *FilePath);
		
		return true;
	}
	else 
	{
		// 파일 읽기 실패
		UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
		return false;
	}
}
//파일 저장
bool UReadWriteText::SaveLocalTxt(TArray<FString> SaveSoundArray, FString FileName)
{
	// 가져온 배열 Join
	FString OutPutString = FString::Join(SaveSoundArray, TEXT(","));

	// 텍스트 파일로 저장
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
