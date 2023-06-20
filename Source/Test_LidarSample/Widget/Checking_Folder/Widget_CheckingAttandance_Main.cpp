// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_CheckingAttandance_Main.h"
#include "Widget_CheckingAttandance_Slot.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "../../GameInstance_Solaseado.h"
#include "../../Pawn_Player.h"
#include "../../ActorComponent_PlayfabStore.h"



UWidget_CheckingAttandance_Main::UWidget_CheckingAttandance_Main(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//슬롯세팅(블루프린트)
	static ConstructorHelpers::FClassFinder<UWidget_CheckingAttandance_Slot> SlotWidget(TEXT("/Game/Project/Widget/Checking/WB_CheckingAttandance_Slot.WB_CheckingAttandance_Slot_C"));
	if (SlotWidget.Succeeded())
	{
		SlotClass = SlotWidget.Class;
	}
}
void UWidget_CheckingAttandance_Main::NativeConstruct()
{
	Get_Reward_BTN->OnClicked.AddDynamic(this, &UWidget_CheckingAttandance_Main::PressReward);

	Begin_Bind_Setting();

	Super::NativeConstruct();
}

void UWidget_CheckingAttandance_Main::Begin_Bind_Setting()
{
	//Reward 테이블 세팅
	UGameInstance_Solaseado* MyInstance = Cast<UGameInstance_Solaseado>(GetGameInstance());
	class UDataTable* Reward_Table = MyInstance->GetCheckingRewardTables();
	// 변수 세팅
	TableLength = Reward_Table->GetRowNames().Num();
	MyPlayer = Cast<APawn_Player>(GetOwningPlayerPawn());

	// 출석일 수 확인 후에
	Checking_Count = MyPlayer->BP_ActorComponent_Playfab->Get_Checking_Count();
	UE_LOG(LogTemp, Log, TEXT("// mycount :: %d "), Checking_Count);

	 
	// 총길이만큼 Slot생성 후 출석일에 따라 Array에 담아줌
	for (int i = 0; i < TableLength; i++)
	{
		UWidget_CheckingAttandance_Slot* SlotWidget = CreateWidget<UWidget_CheckingAttandance_Slot>(GetWorld(), SlotClass);
		// Slot당  출석 검사
		(i < Checking_Count) ? SlotWidget->Set_Slot(i,true): SlotWidget->Set_Slot(i,false);
		SlotArray.Add(SlotWidget); // 배열에 담아주고
		int row = i / 10;
		int Column = i % 10;
		Main_gridPanel->AddChildToUniformGrid(SlotWidget, row, Column);
	}

	//오늘날짜 보상 세팅
	FName CountSTR(*FString::FromInt(Checking_Count + 1));
	TodayReward = *Reward_Table->FindRow<FChecking_Reward>(CountSTR, "");
	// 출석여부에 따라 보상받기 버튼 활성화
	Get_Reward_BTN->SetIsEnabled(!MyPlayer->BP_ActorComponent_Playfab->Is_Today_Checked());
	// 오늘기준 출석일수
	MyPlayer->BP_ActorComponent_Playfab->Is_Today_Checked() ? TodayCount(Checking_Count) : TodayCount(Checking_Count +1);
	
}

void UWidget_CheckingAttandance_Main::PressReward()
{	// 클릭시 버튼 막아주고
	Get_Reward_BTN->SetIsEnabled(false);
	// 서버에 오늘 체크했다는 표기와 보상 업데이트
	MyPlayer->BP_ActorComponent_Playfab->Update_Check_Change(this,TodayReward.Gold);

}

void UWidget_CheckingAttandance_Main::ChangeSlot()
{
	if (Checking_Count<TableLength)
	{
		SlotArray[Checking_Count]->Set_Slot(Checking_Count,true);
	}
	MyPlayer->BP_ActorComponent_Playfab->UpdateCoin();
}
