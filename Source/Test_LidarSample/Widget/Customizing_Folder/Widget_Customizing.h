// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Widget_Customizing.generated.h"

/**
 * 
 */
UCLASS()
class TEST_LIDARSAMPLE_API UWidget_Customizing : public UUserWidget
{
	GENERATED_BODY()
		
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget), BlueprintReadOnly)
		class UWidget_CustomizingTab* WB_CustomizingTab;
	
		UPROPERTY(EditAnywhere, meta = (BindWidget), BlueprintReadWrite)
		class UButton* Button_Previous;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			TArray<int> ITemIDs;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			int TabNumber;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			bool IsFullShot = false;

		// ITemId, Mesh default ó��, Child Slot�� Init�Լ�(���ε�)
		UFUNCTION(BlueprintCallable)
			void Begin_Bind_Setting();
		
		//���� �ν��Ͻ��� ������ ���̺��� ù��° ���� ã�� ITemIDs ������ Default�� ä���ش�
		UFUNCTION(BlueprintCallable)
		void Default_Set_ItemIDs();
	
};
