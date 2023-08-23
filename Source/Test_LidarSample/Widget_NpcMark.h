// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_NpcMark.generated.h"

/**
 * 
 */
UCLASS()
class TEST_LIDARSAMPLE_API UWidget_NpcMark : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//Npc ÀÌ¸§
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TextBlock_NpcName;
	//È¥Àã¸» ÅØ½ºÆ®
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TextBlock_Soliloquy;
	//È¥Àã¸» ¹Ú½º
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class USizeBox* SizeBox_Soliloquy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UImage* ExclamationMark;
	//Å¸ÀÌÇÎ È¿°ú º¯¼ö
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MyText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Writing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StartTimeStamp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NumCharsToDisplay;
public:
	UFUNCTION(BlueprintCallable)
		void WriteSoliloquy(const FString text);
	UFUNCTION(BlueprintCallable)
		void WriteSoliloquyText();
	UFUNCTION(BlueprintCallable)
		void SetVisibleSoliloquyBox(bool visible);
	UFUNCTION(BlueprintCallable)
		void SetNpcName(const FText NpcName);
	UFUNCTION(BlueprintCallable)
		void SetSoliloquyText(const FText Soliloquy);
	UFUNCTION(BlueprintCallable)
		void ExclamationMarkVisibility(ESlateVisibility Visible, FLinearColor MarkColor);
};