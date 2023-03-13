// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_FacebookAccount.h"
#include "Online.h"
 
// Sets default values for this component's properties
UComponent_FacebookAccount::UComponent_FacebookAccount()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UComponent_FacebookAccount::BeginPlay()
{
	Super::BeginPlay();

	FacebookSubsystem = IOnlineSubsystem::Get("facebook");

	if (FacebookSubsystem)
	{
		FacebookSubsystem->Init();

		FacebookSubsystem->GetIdentityInterface()->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateUObject(this, &UComponent_FacebookAccount::ReturnAuthToken));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT(" // FacebookSubsystem NULL  :: "));

	}
}


// Called every frame
void UComponent_FacebookAccount::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UComponent_FacebookAccount::ReturnAuthToken(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (bWasSuccessful)
	{
		FString FacebookAuthToken = FacebookSubsystem->GetIdentityInterface()->GetAuthToken(0);

		UE_LOG(LogTemp, Log, TEXT(" // Facebook Login Response :: %s "), *FacebookAuthToken);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT(" // ReturnAuthToken Error :: LocalNUm ( %d ) Error ( %s )"), LocalUserNum, *Error);
	}
}

void UComponent_FacebookAccount::FacebookLogin(const FString& UserName, const FString& Password)
{
	FOnlineAccountCredentials* NewFacebookAccount = new FOnlineAccountCredentials;
	NewFacebookAccount->Id = UserName;
	NewFacebookAccount->Token = Password;
	NewFacebookAccount->Type = "facebook";

	FacebookSubsystem->GetIdentityInterface()->Login(0, *NewFacebookAccount);
}