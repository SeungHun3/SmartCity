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
		class UOverlay* Overlay_Previous;
		UPROPERTY(EditAnywhere, meta = (BindWidget), BlueprintReadWrite)
			class UButton* Button_Next;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			TArray<int> ITemIDs;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			int Gender;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			int TabNumber;
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			bool IsFullShot = false;

		// ITemId, Mesh default 처리, Child Slot들 Init함수(바인딩)
		UFUNCTION(BlueprintCallable)
			void Begin_Bind_Setting();
		


		//Default를 가지고 있지않고 클릭시 배열에 담아줘야해서 쓰지 않음
		//게임 인스턴스의 데이터 테이블의 첫번째 열을 찾아 ITemIDs 변수를 Default로 채워준다
		UFUNCTION(BlueprintCallable)
		void Default_Set_ItemIDs();
	
};
