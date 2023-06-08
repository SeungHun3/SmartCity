// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_ShopMain.h"
#include "Widget_ShopSlot.h"


void UWidget_ShopMain::Min_Price_Sort(TArray<class UWidget_ShopSlot*> SlotArray)
{
    // ������ ������ �ִ� ���ݼ����� �������� ����
	for (int i = 0; i < SlotArray.Max(); i++) /*Max == size*/
	{
		SlotArray[i]->Item_Infomation.ItemPrice;

        int minIndex = i;
        for (int j = i + 1; j < SlotArray.Max(); ++j)
        {
            if (SlotArray[j]->Item_Infomation.ItemPrice < SlotArray[minIndex]->Item_Infomation.ItemPrice)
            {
                minIndex = j;
            }
        }
        if (minIndex != i)
        {
            UWidget_ShopSlot* tempSlot = SlotArray[i];
            SlotArray[i] = SlotArray[minIndex];
            SlotArray[minIndex] = tempSlot;
        }
	}

    // ������ �������� �ٽ� �������Ʈ���� ���� Add to ViewPortó��
    SelectClass(0);
}
