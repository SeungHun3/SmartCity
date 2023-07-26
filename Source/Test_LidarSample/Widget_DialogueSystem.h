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
	//NPC ��ȭ ����
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* TextBlock_Message;
	//NPC ��ȭ ����
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* TextBlock_Title;
	//1Key
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UOverlay* Overlay_1Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* Button_Next;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int CurrentPage = 1;
	//���� ��ȭ�� �Ѿ �� �ִ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bOnNext = true;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FText> MessageList;

public:
	virtual void NativeConstruct();

	UFUNCTION()
		void OnNextButtonClicked();
	UFUNCTION(BlueprintCallable)
		virtual void ClearMessageBox();
	UFUNCTION(BlueprintCallable)
		virtual void AddMessageBox(const FText title, const FText& Message);
	UFUNCTION(BlueprintImplementableEvent)
		void NextMessage();
};
