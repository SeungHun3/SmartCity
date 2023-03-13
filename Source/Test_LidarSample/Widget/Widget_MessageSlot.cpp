// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_MessageSlot.h"
#include "Components/TextBlock.h"

void UWidget_MessageSlot::AddMessageSlot(const FString& id, const FString& Message)
{
	TextBlock_ID->SetText(FText::FromString(id));
	TextBlock_Message->SetText(FText::FromString(Message));
}