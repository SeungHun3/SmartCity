// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent_PointOfInterest.h"
#include "GameModeBase_Solaseado.h"


// Sets default values for this component's properties
UActorComponent_PointOfInterest::UActorComponent_PointOfInterest()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UActorComponent_PointOfInterest::BeginPlay()
{
	Super::BeginPlay();

	// ...
	SetPOIActor();
}


// Called every frame
void UActorComponent_PointOfInterest::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AActor* UActorComponent_PointOfInterest::GetOwningActor() const
{
	AActor* OwnerActor = GetOwner();
	return OwnerActor;
}
//관심 지점 액터 세팅
void UActorComponent_PointOfInterest::SetPOIActor()
{
	AGameModeBase_Solaseado* GM_Solaseado = Cast<AGameModeBase_Solaseado>(GetWorld()->GetAuthGameMode());
	if (GM_Solaseado)
	{
		AActor* OwnerActor = GetOwner();
		if (OwnerActor)
		{
			GM_Solaseado->POIActors.Add(OwnerActor);
		}
	}
}
