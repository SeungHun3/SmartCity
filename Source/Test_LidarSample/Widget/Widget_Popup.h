// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Struct_Customizing.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Popup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPopupRunEvent, enum_PopupRun, run);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPopupConfirmEvent, enum_PopupRun, run);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPopupCancelEvent, enum_PopupRun, run);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPopupClear);

UCLASS()
class TEST_LIDARSAMPLE_API UWidget_Popup : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		void CheckPopup(enum_PopupStyle style, FText message, FText title, enum_PopupRun run);
	UFUNCTION(BlueprintCallable)
		void CheckPopup_Caution(FText message, enum_PopupRun run);
	void CheckPopup_SubPanel(enum_PopupStyle style, const FText& message, const FText& title, const FText& subMessage, enum_PopupRun run, int number);
	void setRedText(const FText& red);

	UPROPERTY(EditAnywhere)
		bool bIsClikedNo;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* HorizontalBox_Coin;

	//��� �̹��� ���
	UPROPERTY(meta = (BindWidget))
		class UOverlay* Overlay_Caption;
	//���� �ؽ�Ʈ, ���� �˾�, �Ǹ� ����
	UPROPERTY(meta = (BindWidget))
		class UOverlay* Overlay_Sub;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_SubText;
	// ��� ���� �ؽ�Ʈ 
	UPROPERTY(meta = (BindWidget))
		class UOverlay* Overlay_RedText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_RedText;

	// 1Key
	UPROPERTY()
		FPopupConfirmEvent PopupConfirmEvent;
	// 2Key
	UPROPERTY(BlueprintAssignable)
		FPopupRunEvent PopupRunEvent;
	// 2key
	UPROPERTY(BlueprintAssignable)
		FPopupCancelEvent PopupCancelEvent;

	UPROPERTY()
		FPopupClear PopupClear;

	UPROPERTY(EditAnywhere)
		bool bIsClickedNo;

protected:
	//UPROPERTY(EditAnywhere, meta = (BindWidget))
		//class UCanvasPanel* CanvasPanel_Checkpopup;
	virtual void NativeOnInitialized() override;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UCanvasPanel* CanvasPanel_Checkpopup;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UCanvasPanel* CanvasPanel_1Key;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UCanvasPanel* CanvasPanel_2Key;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UButton* Button_Confirm;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UButton* Button_Yes;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UButton* Button_NO;

	// ����
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* TextBlock_Notice;
	// ����
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* TextBlock_Title;

	// ��ư Ŭ��
	UFUNCTION()
		void ClickedConfirm();
	UFUNCTION()
		void ClickedYes();

	enum_PopupRun PopupRun;

public:
	UFUNCTION()
		void ClickedNo();
	UFUNCTION()
		void MenuOff();
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_CheckPrice(int number);
};