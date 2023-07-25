// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_DialogueSystem.generated.h"

/**
 * 
 */
UCLASS()
class TEST_LIDARSAMPLE_API UWidget_DialogueSystem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//NPC 대화 내용
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* TextBlock_Message;
	//NPC 대화 제목
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* TextBlock_Title;
	//1Key
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UOverlay* Overlay_1Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* Button_Next;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int CurrentPage = 0;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FText> MessageList;

	//다음 대화로 넘어갈 수 있는 상태
	bool bOnNext = true;

public:
	virtual void NativeConstruct();

	UFUNCTION(BlueprintCallable)
		void Begin_Setting();
	UFUNCTION()
		void OnNextButtonClicked();
	UFUNCTION(BlueprintCallable)
		virtual void ClearMessageBox();
	UFUNCTION(BlueprintCallable)
		virtual void AddMessageBox(const FText title, const TArray<FText>& Message);
};
