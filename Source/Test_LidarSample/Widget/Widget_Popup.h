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
	UFUNCTION(BlueprintCallable)
		void setRedText(const FText& red);
	UFUNCTION(BlueprintImplementableEvent)
		void setItemTexture();
	UFUNCTION(BlueprintCallable)
		void setPopupSlotImage();

	UPROPERTY(EditAnywhere)
		bool bIsClikedNo;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* HorizontalBox_Coin;

	//경고 이미지 출력
	UPROPERTY(meta = (BindWidget))
		class UOverlay* Overlay_Caption;
	//서브 텍스트, 숫자 팝업, 판매 가격
	UPROPERTY(meta = (BindWidget))
		class UOverlay* Overlay_Sub;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_SubText;
	// 경고 강조 텍스트 
	UPROPERTY(meta = (BindWidget))
		class UOverlay* Overlay_RedText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_RedText;
	// 이미지
	UPROPERTY(meta = (BindWidget))
		class UOverlay* Overlay_Image;

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

	// 본문
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* TextBlock_Notice;
	// 제목
	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* TextBlock_Title;

	// 버튼 클릭
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