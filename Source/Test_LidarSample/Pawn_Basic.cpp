// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_Basic.h"

// Sets default values
APawn_Basic::APawn_Basic()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ½ºÄÌ·¹Åæ ÄÄÆ÷³ÍÆ® ±¸Á¶¼³Á¤
	Root = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	RootComponent = Root;
	Root->SetRelativeTransform(FTransform(FQuat(0.0f, 0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)));
	Body = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Root);

	Hair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair"));
	Hair->SetupAttachment(Body);

	Top = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Top"));
	Top->SetupAttachment(Body);

	Bottoms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bottoms"));
	Bottoms->SetupAttachment(Body);

	Shoes = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shoes"));
	Shoes->SetupAttachment(Body);

	Body->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
}

// Called when the game starts or when spawned
void APawn_Basic::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APawn_Basic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APawn_Basic::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

