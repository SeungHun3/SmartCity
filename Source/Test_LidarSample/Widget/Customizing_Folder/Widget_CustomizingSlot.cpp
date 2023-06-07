// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_CustomizingSlot.h"
#include "Components/Button.h"
#include "Widget_CustomizingTab.h"
#include "Widget_Customizing.h"

#include "../../Pawn_Player.h"

#include "../../GameModeBase_Solaseado.h"
#include "../../PlayerState_Solaseado.h"

//#include "Kismet/KismetArrayLibrary.h"



int UWidget_CustomizingSlot::GetSlotNumber()
{
	// �θ��������� �� ���Ե��� ��ȣ�� �Ű�
	// ������ ���̺��� ��(�ε�����)���� �����ϱ� ���� �뵵
	return 	GetParent()->GetChildIndex(this) + 1;
}

void UWidget_CustomizingSlot::Init_Slot_Implementation(APawn_Player* MyCostumePawn, const TArray<class UDataTable*>& MyDataTable, UWidget_CustomizingTab* CustomizingTab)
{

	SlotCount = GetSlotNumber();
	CostumePawn = MyCostumePawn;
	DataTable = MyDataTable;
	// Ŀ���͸���¡ �� ������ ��������Ʈ �Լ��� ���ε��ϱ�
	CustomizingTab->Change_Slot_Delegate.AddDynamic(this, &UWidget_CustomizingSlot::Change_Slot);
	// Ŭ���̺�Ʈ ���ε�
	Slot_BTN->OnClicked.AddDynamic(this, &UWidget_CustomizingSlot::ClickEvent);

}

void UWidget_CustomizingSlot::Change_Slot_Implementation(int TabNumber)
{
	//TabNumber�� ���� ���������̺� �迭 ����-> Change Image
	FName rowName = FName(*FString::FromInt(SlotCount));
	Customizing_Struct = *DataTable[TabNumber]->FindRow<FCustomizing_Struct>(rowName,"");
	
	//�������Ʈ���� �̹��� �ٲ� �� �ְ� BP �۾����� �ѱ�
	Change_Image(Customizing_Struct.Image);

}

void UWidget_CustomizingSlot::ClickEvent()
{	
	
	//GM�� �ִ� PlayerState�� �����ͼ� ������ ������

	AGameModeBase_Solaseado* GM_Solaseado = Cast<AGameModeBase_Solaseado>(GetWorld()->GetAuthGameMode());
	APlayerState_Solaseado* PS = GM_Solaseado->get_PlayerState_Solaseado();
	
	//���� ������ ���� ����ü�� ������ ID���� �ٲ�־��ش�
	PS->Widget_Customizing->ITemIDs[PS->Widget_Customizing->TabNumber] = FCString::Atoi(*Customizing_Struct.ItemID);
	
	// ���� �ִ� �޽��� ����ü�� ������ ä���־��ش�
	CostumePawn->ChangeMesh(Customizing_Struct.ClassName, Customizing_Struct.Mesh);

}

