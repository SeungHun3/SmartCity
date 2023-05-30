// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_UserInterfaceWidgetManager.h"
#include "Components/WidgetComponent.h"

#include "ActorComponent_Popup.h"
#include "Widget_Popup.h"

// Sets default values
AActor_UserInterfaceWidgetManager::AActor_UserInterfaceWidgetManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActor_UserInterfaceWidgetManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActor_UserInterfaceWidgetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AActor_UserInterfaceWidgetManager::setEnableCollisionChecker()
{
	bool bChecker_SlotMenu = true;
	if (CurrentCheckmenu == enum_Menu::Defualt)
	{
		// ÇöÀç ¿­¸° ½½·Ô (¸Þ´º)
		bChecker_SlotMenu = false;
	}

	if (bChecker_Keyboard || bChecker_Popup || bChecker_MainMenu || bChecker_SlotMenu)
	{
		this->SetActorEnableCollision(true);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("// Checker_EnableCollision UI :: false"))
			this->SetActorEnableCollision(false);
	}
}
//ÆË¾÷ À§Á¬
void AActor_UserInterfaceWidgetManager::AddPopupWidget(UWidgetComponent* target, enum_PopupStyle style, enum_PopupRun run, FText message, FText title)
{
	bChecker_Popup = true;

	if (target == nullptr)
		PopupComp->AddPopupWidget(PopupWidgetComp, style, run, message, title, PlayerMainMenuWidgetList);
	else
		PopupComp->AddPopupWidget(target, style, run, message, title, PlayerMainMenuWidgetList);

	setEnableCollisionChecker();
}
// ÆË¿ø À§Á¬
void AActor_UserInterfaceWidgetManager::AddPopupWidget_Caution(UWidgetComponent* target, enum_PopupRun run, FText message, FText CautionText)
{
	bChecker_Popup = true;

	if (target == nullptr)
		PopupComp->AddPopupWidget_Caution(PopupWidgetComp, run, message, CautionText, PlayerMainMenuWidgetList);
	else
		PopupComp->AddPopupWidget_Caution(target, run, message, CautionText, PlayerMainMenuWidgetList);

	setEnableCollisionChecker();
}
//ÆË¾÷ À§Á¬
void AActor_UserInterfaceWidgetManager::AddPopupWidget_SubPanel(UWidgetComponent* target, enum_PopupStyle style, enum_PopupRun run,
	FText message, FText title, FText subtitle, FText Cautiontext, int Number)
{
	bChecker_Popup = true;

	if (target == nullptr)
		PopupComp->AddPopupWidget_SubPanel(PopupWidgetComp, style, run, message, title, subtitle, Number, Cautiontext, PlayerMainMenuWidgetList);
	else
		PopupComp->AddPopupWidget_SubPanel(target, style, run, message, title, subtitle, Number, Cautiontext, PlayerMainMenuWidgetList);

	setEnableCollisionChecker();
}