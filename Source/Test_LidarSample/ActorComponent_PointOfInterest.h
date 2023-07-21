// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorComponent_PointOfInterest.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_LIDARSAMPLE_API UActorComponent_PointOfInterest : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActorComponent_PointOfInterest();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyComponent")
		bool IsStatic;
	// 관심지점의 아이콘 이미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyComponent")
		UTexture2D* IconImage;
	UFUNCTION(BlueprintCallable, Category = "MyComponent")
		AActor* GetOwningActor() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "MyActors")
		void SetPOIActor();
};