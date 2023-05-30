// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Struct_Customizing.h"
#include "Components/ActorComponent.h"
#include "Widget_Popup.h"
#include "ActorComponent_Popup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClearPopupWidgetEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCreateCharacter);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FItemSellEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoticeConfirmEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPopupRun, enum_PopupRun, popup);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_LIDARSAMPLE_API UActorComponent_Popup : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActorComponent_Popup();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UUserWidget> PopupWidget;
	
	UPROPERTY()
		class UWidget_Popup* Popup;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	class UWidgetComponent* targetComp;

	// Enabled Widget List
	TArray<UUserWidget*> WidgetArray;

	// call popup evnet : 2key 
	UFUNCTION()
		void PopupRunEvent(enum_PopupRun event);
	
	// call popup evnet : 2key cancel
	UFUNCTION()
		void PopupCancelEvent(enum_PopupRun Run);
	// call popup evnet : 1key 
	UFUNCTION()
		void PopupConfirmEvent(enum_PopupRun Run);

	UFUNCTION()
		void ClearPopupWidget();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CreatePopupWidget(class UWidgetComponent* target, const TArray<UUserWidget*>& Enabledwidget);

	UPROPERTY(BlueprintAssignable)
		FClearPopupWidgetEvent ClearPopupWidgetEvent;
	UPROPERTY(BlueprintAssignable)
		FCreateCharacter CreateCharacter;
	//UPROPERTY(BlueprintAssingable)
		//FItemSellEvent ItemSellEvent;
	UPROPERTY(BlueprintAssignable)
		FNoticeConfirmEvent NoticeConfirmEvent;
	// Add POPUP Widget
	UFUNCTION(BlueprintCallable)
		void AddPopupWidget(class UWidgetComponent* target, enum_PopupStyle style, enum_PopupRun run,
			FText message, FText title, const TArray<UUserWidget*>& Enabledwidget);
	// Add POPUP Widget + Caution
	UFUNCTION(BlueprintCallable)
		void AddPopupWidget_Caution(UWidgetComponent* target, enum_PopupRun run,
			FText message, FText CautionText, const TArray<UUserWidget*>& Enabledwidget);
	// Add POPUP Widget + SubPanel + Caution
	UFUNCTION(BlueprintCallable)
		void AddPopupWidget_SubPanel(UWidgetComponent* target, enum_PopupStyle style, enum_PopupRun run,
			FText message, FText title, FText subMessage, int number, FText CautionText, const TArray<UUserWidget*>& Enabledwidget);
	/*
	void CreatePopupWidget(class UWidgetComponent* target, const TArray<UUserWidget*>& Enabledwidget);
	
	
	UPROPERTY(BlueprintAssignable)
		FPopupRun PopupRun;
	*/
};