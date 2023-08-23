// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_NpcMark.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Kismet/KismetSystemLibrary.h"
#include <cmath>

void UWidget_NpcMark::WriteSoliloquy(const FString text)
{
	UWorld* World = GetWorld();
	if (!Writing) 
	{
		Writing = true;
		MyText = text;
		if (World) 
		{
			StartTimeStamp = UKismetSystemLibrary::GetGameTimeInSeconds(World);
		}
		TextBlock_Soliloquy->SetText(FText::GetEmpty());
	}
}
//Npc 혼잣말 타이핑 효과
void UWidget_NpcMark::WriteSoliloquyText()
{
	UWorld* World = GetWorld();

	float DeltaTime = (UKismetSystemLibrary::GetGameTimeInSeconds(World) - StartTimeStamp) * 10;
	NumCharsToDisplay = FMath::Clamp(static_cast<int32>(std::floor(DeltaTime)), 0, MyText.Len());

	FString Substring = MyText.Mid(0, NumCharsToDisplay);
	TextBlock_Soliloquy->SetText(FText::FromString(Substring));
	//작성완료 확인
	if (NumCharsToDisplay >= MyText.Len()) 
	{
		Writing = false;
	}
}
//혼잣말 박스 설정
void UWidget_NpcMark::SetVisibleSoliloquyBox(bool visible)
{
	if (visible) 
	{
		SizeBox_Soliloquy->SetVisibility(ESlateVisibility::Visible);
	}
	else 
	{
		SizeBox_Soliloquy->SetVisibility(ESlateVisibility::Collapsed);
	}
}
//Npc이름 설정
void UWidget_NpcMark::SetNpcName(const FText NpcName)
{
	TextBlock_NpcName->SetText(NpcName);
}
//혼잣말 텍스트 설정
void UWidget_NpcMark::SetSoliloquyText(const FText Soliloquy)
{
	TextBlock_Soliloquy->SetText(Soliloquy);
}
//Npc 마크 느낌표 설정
void UWidget_NpcMark::ExclamationMarkVisibility(ESlateVisibility Visible, FLinearColor MarkColor)
{
	ExclamationMark->SetVisibility(Visible);
	ExclamationMark->SetBrushTintColor(MarkColor);
}
