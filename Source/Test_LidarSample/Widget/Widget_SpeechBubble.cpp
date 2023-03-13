// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_SpeechBubble.h"
#include "Components/TextBlock.h"

void UWidget_SpeechBubble::setSpeechBox(const FText& message)
{
	TextBlock_Message->SetText(message);

}
