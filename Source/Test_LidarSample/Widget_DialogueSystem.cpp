// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_DialogueSystem.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "GameInstance_Solaseado.h"

void UWidget_DialogueSystem::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Next->OnClicked.AddUniqueDynamic(this, &UWidget_DialogueSystem::OnNextButtonClicked);
}
//������ư Ŭ��
void UWidget_DialogueSystem::OnNextButtonClicked()
{
	if (bOnNext)
	{
		CurrentPage++;
		NextMessage();
	}
}
//�޼��� �ʱ�ȭ
void UWidget_DialogueSystem::ClearMessageBox()
{
	SetVisibility(ESlateVisibility::Collapsed);

	CurrentPage = 1;
}

void UWidget_DialogueSystem::AddMessageBox(const FText title, const FText& Message)
{
	SetVisibility(ESlateVisibility::Visible);
	TextBlock_Title->SetText(title);
	TextBlock_Message->SetText(Message);
}
