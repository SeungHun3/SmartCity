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
	//���Լ���(�������Ʈ)
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
	//Reward ���̺� ����
	UGameInstance_Solaseado* MyInstance = Cast<UGameInstance_Solaseado>(GetGameInstance());
	class UDataTable* Reward_Table = MyInstance->GetCheckingRewardTables();
	// ���� ����
	TableLength = Reward_Table->GetRowNames().Num();
	MyPlayer = Cast<APawn_Player>(GetOwningPlayerPawn());

	// �⼮�� �� Ȯ�� �Ŀ�
	Checking_Count = MyPlayer->BP_ActorComponent_Playfab->Get_Checking_Count();
	UE_LOG(LogTemp, Log, TEXT("// mycount :: %d "), Checking_Count);

	 
	// �ѱ��̸�ŭ Slot���� �� �⼮�Ͽ� ���� Array�� �����
	for (int i = 0; i < TableLength; i++)
	{
		UWidget_CheckingAttandance_Slot* SlotWidget = CreateWidget<UWidget_CheckingAttandance_Slot>(GetWorld(), SlotClass);
		// Slot��  �⼮ �˻�
		(i < Checking_Count) ? SlotWidget->Set_Slot(i,true): SlotWidget->Set_Slot(i,false);
		SlotArray.Add(SlotWidget); // �迭�� ����ְ�
		int row = i / 10;
		int Column = i % 10;
		Main_gridPanel->AddChildToUniformGrid(SlotWidget, row, Column);
	}

	//���ó�¥ ���� ����
	FName CountSTR(*FString::FromInt(Checking_Count + 1));
	TodayReward = *Reward_Table->FindRow<FChecking_Reward>(CountSTR, "");
	// �⼮���ο� ���� ����ޱ� ��ư Ȱ��ȭ
	Get_Reward_BTN->SetIsEnabled(!MyPlayer->BP_ActorComponent_Playfab->Is_Today_Checked());
	// ���ñ��� �⼮�ϼ�
	MyPlayer->BP_ActorComponent_Playfab->Is_Today_Checked() ? TodayCount(Checking_Count) : TodayCount(Checking_Count +1);
	
}

void UWidget_CheckingAttandance_Main::PressReward()
{	// Ŭ���� ��ư �����ְ�
	Get_Reward_BTN->SetIsEnabled(false);
	// ������ ���� üũ�ߴٴ� ǥ��� ���� ������Ʈ
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
