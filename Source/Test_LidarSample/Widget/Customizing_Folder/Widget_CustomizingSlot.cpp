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
	// 부모위젯에서 각 슬롯들의 번호를 매겨
	// 데이터 테이블의 행(인덱스값)으로 나열하기 위한 용도
	return 	GetParent()->GetChildIndex(this) + 1;
}

void UWidget_CustomizingSlot::Init_Slot_Implementation(APawn_Player* MyCostumePawn, const TArray<class UDataTable*>& MyDataTable, UWidget_CustomizingTab* CustomizingTab)
{

	SlotCount = GetSlotNumber();
	CostumePawn = MyCostumePawn;
	DataTable = MyDataTable;
	// 커스터마이징 탭 위젯의 델리게이트 함수에 바인딩하기
	CustomizingTab->Change_Slot_Delegate.AddDynamic(this, &UWidget_CustomizingSlot::Change_Slot);
	// 클릭이벤트 바인딩
	Slot_BTN->OnClicked.AddDynamic(this, &UWidget_CustomizingSlot::ClickEvent);

}

void UWidget_CustomizingSlot::Change_Slot_Implementation(int TabNumber)
{
	//TabNumber를 통해 데이터테이블 배열 접근-> Change Image
	FName rowName = FName(*FString::FromInt(SlotCount));
	Customizing_Struct = *DataTable[TabNumber]->FindRow<FCustomizing_Struct>(rowName,"");
	
	//블루프린트에서 이미지 바꿀 수 있게 BP 작업으로 넘김
	Change_Image(Customizing_Struct.Image);

}

void UWidget_CustomizingSlot::ClickEvent()
{	
	
	//GM에 있는 PlayerState를 가져와서 위젯을 꺼내고

	AGameModeBase_Solaseado* GM_Solaseado = Cast<AGameModeBase_Solaseado>(GetWorld()->GetAuthGameMode());
	APlayerState_Solaseado* PS = GM_Solaseado->get_PlayerState_Solaseado();
	
	//현재 위젯의 값에 구조체의 아이템 ID값을 바꿔넣어준다
	PS->Widget_Customizing->ITemIDs[PS->Widget_Customizing->TabNumber] = FCString::Atoi(*Customizing_Struct.ItemID);
	
	// 폰에 있는 메쉬도 구조체의 값으로 채워넣어준다
	CostumePawn->ChangeMesh(Customizing_Struct.ClassName, Customizing_Struct.Mesh);

}

