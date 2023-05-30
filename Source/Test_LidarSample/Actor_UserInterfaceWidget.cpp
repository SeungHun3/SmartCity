// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_UserInterfaceWidget.h"

// Sets default values
AActor_UserInterfaceWidget::AActor_UserInterfaceWidget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActor_UserInterfaceWidget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActor_UserInterfaceWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

