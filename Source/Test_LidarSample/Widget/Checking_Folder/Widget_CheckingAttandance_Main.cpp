// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_CheckingAttandance_Main.h"
#include "Widget_CheckingAttandance_Slot.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"


#include "../../GameInstance_Solaseado.h"
#include "../../Struct_Customizing.h"
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
	UDataTable* Reward_Table = MyInstance->GetCheckingRewardTables();
	// ���� ����
	TableLength = Reward_Table->GetRowNames().Num();
	MyPlayer = Cast<APawn_Player>(GetOwningPlayerPawn());

	// �⼮�� �� Ȯ�� �Ŀ� 
	int Checking_Count = MyPlayer->BP_ActorComponent_Playfab->Get_Checking_Count();
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

	// �⼮���ο� ���� ����ޱ� ��ư Ȱ��ȭ
	Get_Reward_BTN->SetIsEnabled(!MyPlayer->BP_ActorComponent_Playfab->Is_Today_Checked());

	// ������ //���� �Լ��� �ּ������ ��
	//Get_Reward_BTN->SetIsEnabled(MyPlayer->BP_ActorComponent_Playfab->Is_Today_Checked());


}

void UWidget_CheckingAttandance_Main::PressReward()
{	// Ŭ���� ������ ���� üũ�ߴٴ� ǥ��� ���� ������Ʈ
	MyPlayer->BP_ActorComponent_Playfab->Update_Check_Change(this);
}

void UWidget_CheckingAttandance_Main::ChangeSlot()
{
	int Checking_Count = MyPlayer->BP_ActorComponent_Playfab->Get_Checking_Count();
	if (Checking_Count-1<TableLength)
	{
		SlotArray[Checking_Count-1]->Set_Slot(Checking_Count,true);
	}
}
