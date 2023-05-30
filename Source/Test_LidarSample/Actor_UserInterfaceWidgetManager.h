// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Struct_Customizing.h"
#include "Actor_UserInterfaceWidgetManager.generated.h"

UENUM(BlueprintType)
enum class enum_Menu : uint8
{
	Channel = 0,
	Inventory,
	CreateRoom,
	JoinRoom,
	Mission,
	Community,
	Option,
	LeaveRoom,
	Exit,
	Store,
	Quest,
	WorldMap,
	Defualt,
};

UCLASS()
class TEST_LIDARSAMPLE_API AActor_UserInterfaceWidgetManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_UserInterfaceWidgetManager();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UWidgetComponent* PopupWidgetComp;

	// 현재 열린 메뉴
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		enum_Menu CurrentCheckmenu = enum_Menu::Defualt;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		TArray<UUserWidget*> PlayerMainMenuWidgetList;

	// popup comp
	class UActorComponent_Popup* PopupComp;

	bool bChecker_Popup = false;
	bool bChecker_Keyboard = false;
	bool bChecker_MainMenu = false;

	void setEnableCollisionChecker();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//팝업
	UFUNCTION(BlueprintCallable)
		void AddPopupWidget(class UWidgetComponent* target, enum_PopupStyle style, enum_PopupRun run, FText message, FText title);
	UFUNCTION(BlueprintCallable)
		void AddPopupWidget_Caution(UWidgetComponent* target, enum_PopupRun run, FText message, FText CautionText = FText());
	UFUNCTION(BlueprintCallable)
		void AddPopupWidget_SubPanel(UWidgetComponent* target, enum_PopupStyle style, enum_PopupRun run,
			FText message, FText title, FText subtitle, FText Cautiontext = FText(), int Number = 0);
};
