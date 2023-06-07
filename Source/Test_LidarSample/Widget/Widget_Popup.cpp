// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_Popup.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/Overlay.h"
#include "Components/HorizontalBox.h"

void UWidget_Popup::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Confirm->OnClicked.AddDynamic(this, &UWidget_Popup::ClickedConfirm);
	Button_NO->OnClicked.AddDynamic(this, &UWidget_Popup::ClickedNo);
	Button_Yes->OnClicked.AddDynamic(this, &UWidget_Popup::ClickedYes);
}
//1Key 확인 버튼
void UWidget_Popup::ClickedConfirm()
{
	PopupClear.Broadcast();
	PopupConfirmEvent.Broadcast(PopupRun);
}
//2Key 취소 버튼
void UWidget_Popup::ClickedNo()
{
	bIsClickedNo = true;
	if (PopupRun != enum_PopupRun::Tutorial)
	{
		PopupClear.Broadcast();
		PopupCancelEvent.Broadcast(PopupRun);
	}
}
//2key 확인 버튼 : 이벤트 처리
void UWidget_Popup::ClickedYes()
{
	//PopupRun
	PopupRunEvent.Broadcast(PopupRun);
	PopupClear.Broadcast();
}
void UWidget_Popup::MenuOff()
{
	bIsClikedNo = false;
	PopupClear.Broadcast();
	PopupCancelEvent.Broadcast(PopupRun);
}
//팝업창 style 결정
void UWidget_Popup::CheckPopup(enum_PopupStyle style, FText message, FText title, enum_PopupRun run)
{
	//경고
	Overlay_Caption->SetVisibility(ESlateVisibility::Collapsed);
	// 서브 텍스트, 가격 표기 추가
	Overlay_Sub->SetVisibility(ESlateVisibility::Collapsed);
	// 경고문
	Overlay_RedText->SetVisibility(ESlateVisibility::Collapsed);

	CanvasPanel_Checkpopup->SetVisibility(ESlateVisibility::Visible);
	switch (style)
	{
		case enum_PopupStyle::Popup1Key:
		{
			CanvasPanel_1Key->SetVisibility(ESlateVisibility::Visible);
			CanvasPanel_2Key->SetVisibility(ESlateVisibility::Collapsed);
			break;
		}
		case enum_PopupStyle::Popup2Key:
		{
			CanvasPanel_1Key->SetVisibility(ESlateVisibility::Collapsed);
			CanvasPanel_2Key->SetVisibility(ESlateVisibility::Visible);
			break;
		}
	}
	PopupRun = run;
	TextBlock_Notice->SetText(message);
	TextBlock_Title->SetText(title);
}
// 경고 팝업
void UWidget_Popup::CheckPopup_Caution(const FText& message, enum_PopupRun run)
{
	CheckPopup(enum_PopupStyle::Popup1Key, message, FText::FromString(FString("Caution")), run);
	Overlay_Caption->SetVisibility(ESlateVisibility::HitTestInvisible);
}
// 서브 팝업 , 숫자 팝업
void UWidget_Popup::CheckPopup_SubPanel(enum_PopupStyle style, const FText& message, const FText& title, const FText& subMessage, enum_PopupRun run, int number)
{
	CheckPopup(style, message, title, run);
	Overlay_Sub->SetVisibility(ESlateVisibility::HitTestInvisible);
	TextBlock_SubText->SetText(subMessage);
	// 숫자 표기
	if (number != 0)
	{
		Blueprint_CheckPrice(number);
		HorizontalBox_Coin->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		HorizontalBox_Coin->SetVisibility(ESlateVisibility::Collapsed);
	}
}
//  경고 문구 추가 
void UWidget_Popup::setRedText(const FText& red)
{
	if (red.IsEmpty())
	{
		Overlay_RedText->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Overlay_RedText->SetVisibility(ESlateVisibility::HitTestInvisible);
		TextBlock_RedText->SetText(red);
	}
}