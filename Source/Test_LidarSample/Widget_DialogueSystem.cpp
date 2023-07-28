// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_DialogueSystem.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Overlay.h"

#include "GameInstance_Solaseado.h"

void UWidget_DialogueSystem::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Next->OnClicked.AddUniqueDynamic(this, &UWidget_DialogueSystem::OnNextButtonClicked);
	Button_First->OnClicked.AddUniqueDynamic(this, &UWidget_DialogueSystem::OnFirstButtonClicked);
	Button_Second->OnClicked.AddUniqueDynamic(this, &UWidget_DialogueSystem::OnSecondButtonClicked);
}
//1key
//���� ��ư Ŭ��
void UWidget_DialogueSystem::OnNextButtonClicked()
{
	if (bOnNext)
	{
		CurrentPage++;
		NextMessage();
	}
}
//2Key �߿���
// ù ��° ��ư ���ε� �Լ�
void UWidget_DialogueSystem::OnFirstButtonClicked()
{
	//UE_LOG(LogTemp, Warning, TEXT("First Button"));
}
// �� ��° ��ư ���ε� �Լ�
void UWidget_DialogueSystem::OnSecondButtonClicked()
{
	//UE_LOG(LogTemp, Warning, TEXT("Second Button"));
}
//�޼��� �ʱ�ȭ
void UWidget_DialogueSystem::ClearMessageBox()
{
	SetVisibility(ESlateVisibility::Collapsed);

	CurrentPage = 1;
}
//��ȭâ ��Ÿ�� ����, ���� �߰�
void UWidget_DialogueSystem::AddMessageBox(enum_DialogueStyle style, const FText title, const FText& Message, const FText& FirstBtnText, const FText& SecondBtnText)
{
	SetVisibility(ESlateVisibility::Visible);

	TextBlock_Title->SetText(title);
	TextBlock_Message->SetText(Message);
	Button_FirstText->SetText(FirstBtnText);
	Button_SecondText->SetText(SecondBtnText);

	switch (style)
	{
		case enum_DialogueStyle::Dialogue1Key:
		{
			Overlay_1Key->SetVisibility(ESlateVisibility::Visible);
			Overlay_2Key->SetVisibility(ESlateVisibility::Collapsed);
			break;
		}
		case enum_DialogueStyle::Dialogue2Key:
		{
			Overlay_1Key->SetVisibility(ESlateVisibility::Collapsed);
			Overlay_2Key->SetVisibility(ESlateVisibility::Visible);
			break;
		}
	}
}