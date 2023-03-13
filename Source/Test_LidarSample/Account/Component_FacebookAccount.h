// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// #include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Component_FacebookAccount.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_LIDARSAMPLE_API UComponent_FacebookAccount : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UComponent_FacebookAccount();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	class IOnlineSubsystem* FacebookSubsystem;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ReturnAuthToken(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	UFUNCTION(BlueprintCallable)
		void FacebookLogin(const FString& UserName, const FString& Password);
		
};
