// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_ShopMain.h"
#include "Widget_ShopSlot.h"


void UWidget_ShopMain::Min_Price_Sort(TArray<class UWidget_ShopSlot*> SlotArray)
{
    // 위젯이 가지고 있는 가격순으로 내림차순 정렬
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

    // 정렬이 끝났으면 다시 블루프린트에서 위젯 Add to ViewPort처리
    SelectClass(0);
}
