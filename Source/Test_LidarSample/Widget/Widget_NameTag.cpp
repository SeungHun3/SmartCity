// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_NameTag.h"
#include "Components/TextBlock.h"

void UWidget_NameTag::setPlayerNametag(const FString& name)
{
	TextBlock_NameTag->SetText(FText::FromString(name));
}
