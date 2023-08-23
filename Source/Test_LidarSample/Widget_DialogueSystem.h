// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Struct_Customizing.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TextBlock_Message;
	//NPC 대화 제목
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TextBlock_Title;
	//1Key
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UOverlay* Overlay_1Key;
	//2Key
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UOverlay* Overlay_2Key;
	//1Key Button
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* Button_Next;
	//2Key Button
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* Button_First;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* Button_FirstText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* Button_Second;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* Button_SecondText;
	//대화 스킵 버튼
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* Button_Skip;
	//현재 페이지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int CurrentPage = 1;
	//다음 대화로 넘어갈 수 있는 상태
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bOnNext = true;
	//퀘스트 대화 중인지 아닌지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsQuestDialogue = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UDataTable* QuestTable;
	//퀘스트 버튼
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* Button_Accept;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UButton* Button_Drop;
public:
	//타이핑 상태
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Writing = false;
	//타이핑 효과 텍스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MyText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StartTimestamp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 NumCharsToDisplay;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FText> MessageList;

public:
	virtual void NativeConstruct();
	//1Key 버튼
	UFUNCTION()
		void OnNextButtonClicked();
	//2Key 버튼
	UFUNCTION()
		void OnFirstButtonClicked();
	UFUNCTION()
		void OnSecondButtonClicked();
	UFUNCTION(BlueprintCallable)
		virtual void RemoveMessageBox();
	UFUNCTION(BlueprintCallable)
		void PageReset();
	UFUNCTION(BlueprintCallable)
		virtual void AddMessageBox(enum_DialogueStyle style, const FText title, const FText& Message, const FText& FirstBtnText, const FText& SecondBtnText);
	UFUNCTION(BlueprintImplementableEvent)
		void NextMessage();
	UFUNCTION(BlueprintCallable)
		void SetAllButton(bool IsEnabled);
	UFUNCTION(BlueprintCallable)
		void Write(const FString text);
	UFUNCTION(BlueprintCallable)
		void WriteText();
};
