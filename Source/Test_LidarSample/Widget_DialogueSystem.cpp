// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_DialogueSystem.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "GameInstance_Solaseado.h"

void UWidget_DialogueSystem::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Next->OnClicked.AddUniqueDynamic(this, &UWidget_DialogueSystem::OnNextButtonClicked);
	Begin_Setting();
}
void UWidget_DialogueSystem::Begin_Setting()
{
	UGameInstance_Solaseado* MyInstance = Cast<UGameInstance_Solaseado>(GetGameInstance());
	class UDataTable* DialogueNpc_Table = MyInstance->GetDialogueNpcTable();
}
//다음버튼 클릭
void UWidget_DialogueSystem::OnNextButtonClicked()
{
	if (bOnNext)
	{
		CurrentPage++;
	}
}
//메세지 초기화
void UWidget_DialogueSystem::ClearMessageBox()
{
	RemoveFromParent();

	SetVisibility(ESlateVisibility::Collapsed);

	CurrentPage = 0;
}

void UWidget_DialogueSystem::AddMessageBox(const FText title, const TArray<FText>& Message)
{
	TextBlock_Title->SetText(title);

	MessageList = Message;
}
