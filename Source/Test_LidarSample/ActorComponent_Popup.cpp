// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent_Popup.h"
#include "Components/WidgetComponent.h"
#include "Widget_Popup.h"

// Sets default values for this component's properties
UActorComponent_Popup::UActorComponent_Popup()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UActorComponent_Popup::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UActorComponent_Popup::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
//기본 팝업
void UActorComponent_Popup::AddPopupWidget(UWidgetComponent* target, enum_PopupStyle style, enum_PopupRun run,
	FText message, FText title, const TArray<UUserWidget*>& EnabledWidget)
{
	CreatePopupWidget(target, EnabledWidget);

	Popup->CheckPopup(style, message, title, run); // 일반
}
void UActorComponent_Popup::AddPopupWidget_Caution(UWidgetComponent* target, enum_PopupRun run,
	FText message, FText CautionText, const TArray<UUserWidget*>& Enabledwidget)
{
	CreatePopupWidget(target, Enabledwidget);
	Popup->CheckPopup_Caution(message, run); // 경고
	if (!CautionText.IsEmpty())
		Popup->setRedText(CautionText); // 경고 메세지 추가
}
// 팝업 + 서브 패널 + 경고 메세지 추가
void UActorComponent_Popup::AddPopupWidget_SubPanel(UWidgetComponent* target, enum_PopupStyle style, enum_PopupRun run,
	FText message, FText title, FText subMessage, int number, FText CautionText, const TArray<UUserWidget*>& Enabledwidget)
{
	CreatePopupWidget(target, Enabledwidget);
	Popup->CheckPopup_SubPanel(style, message, title, subMessage, run, number); // 경고
	Popup->setRedText(CautionText); // 경고 메세지 추가
}
void UActorComponent_Popup::CreatePopupWidget(class UWidgetComponent* target, const TArray<UUserWidget*>& Enabledwidget)
{
	targetComp = target;
	if (!Popup)
	{
		Popup = CreateWidget<UWidget_Popup>(GetWorld(), PopupWidget);
		if (Popup)
		{
			Popup->PopupClear.AddDynamic(this, &UActorComponent_Popup::ClearPopupWidget);
			Popup->PopupRunEvent.AddDynamic(this, &UActorComponent_Popup::PopupRunEvent);
			Popup->PopupConfirmEvent.AddDynamic(this, &UActorComponent_Popup::PopupConfirmEvent);

			Popup->PopupCancelEvent.AddDynamic(this, &UActorComponent_Popup::PopupCancelEvent);
			targetComp->SetWidget(Popup);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "// PopupWidget :: NULL Object ");
			return;
		}
	}
	if (targetComp)
	{
		targetComp->SetVisibility(true);
		targetComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
		Popup->AddToViewport(100);

	for (auto it : WidgetArray)
	{
		if (it)
		{
			it->SetIsEnabled(true);
		}
	}
	WidgetArray = Enabledwidget;

	for (auto it : WidgetArray)
	{
		if (it)
		{
			it->SetIsEnabled(false);
		}
	}
}
void UActorComponent_Popup::ClearPopupWidget()
{
	for (auto it : WidgetArray)
	{
		if (it)
			it->SetIsEnabled(true);
	}
	if (targetComp)
	{
		targetComp->SetVisibility(false);
		targetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
		Popup->RemoveFromParent();

	WidgetArray.Reset();
	WidgetArray.SetNum(0);
	// 블루프린트 확인
	ClearPopupWidgetEvent.Broadcast();
}
void UActorComponent_Popup::PopupRunEvent(enum_PopupRun RunEvent)
{
	switch (RunEvent)
	{
		case enum_PopupRun::Exit: 
		{
			break;
		}
		case enum_PopupRun::CreateCharacter:
		{
			CreateCharacter.Broadcast();
			break;
		}
		case enum_PopupRun::Sell:
		{
			//ItemSellEvent.Broadcast();
			break;
		}
		case enum_PopupRun::Cancel:
		{
			ClearPopupWidget();
			break;
		}

	}
	//PopupRun.Broadcast(RunEvent);
}
// 2key 확인버튼
void UActorComponent_Popup::PopupConfirmEvent(enum_PopupRun Run)
{
	switch (Run)
	{
		case enum_PopupRun::Notice:
		{
			NoticeConfirmEvent.Broadcast();
			break;
		}
		case enum_PopupRun::Cancel:
			break;
	}
	ClearPopupWidget();
}
// 2key 취소버튼
void UActorComponent_Popup::PopupCancelEvent(enum_PopupRun Run)
{
	
	ClearPopupWidget();
}