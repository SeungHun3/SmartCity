#include "Pawn_NPC.h"
#include "GameModeBase_Solaseado.h"
#include "Actor_SolaseadoPhoton.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"


// Sets default values
APawn_NPC::APawn_NPC()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = Root;
	Root->SetRelativeTransform(FTransform(FQuat(0.0f, 0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)));


	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	CapsuleComp->SetupAttachment(Root);

	BodyComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	BodyComp->SetupAttachment(CapsuleComp);
	BodyComp->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));

	Hair = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair"));
	Hair->SetupAttachment(BodyComp);

	Top = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Top"));
	Top->SetupAttachment(BodyComp);

	Bottoms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bottoms"));
	Bottoms->SetupAttachment(BodyComp);

	Shoes = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shoes"));
	Shoes->SetupAttachment(BodyComp);

	NameTagComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameTagComp"));
	NameTagComp->SetupAttachment(Root);

	MessageComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("MessageBox"));
	MessageComp->SetupAttachment(Root);
	//MessageComp->SetVisibility(false);
	MessageComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);



}


// Called when the game starts or when spawned
void APawn_NPC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APawn_NPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void APawn_NPC::setOutline()
{
}

void APawn_NPC::ClearOutline()
{
}

void APawn_NPC::SetName(const FText& Name)
{
}

void APawn_NPC::OnNameTag()
{

}

void APawn_NPC::SelfDestroy()
{
	this->Destroy();
}

void APawn_NPC::NPCInteraction()
{
	if (!bIntercation)
	{

	}
}

//이동함수 : eNPCType로 멀티 유무를 가려서 이동시켜준다.
void APawn_NPC::NPCMoveToLocation(FVector FLoc)
{
	if (eNPCType == enum_NPCMultiType::Multi)
	{
		AGameModeBase_Solaseado* GM_Solaseado = Cast<AGameModeBase_Solaseado>(GetWorld()->GetAuthGameMode());
		GM_Solaseado->PhotonCloud->NPCMove(this, FLoc);
	}
	NPCMove(FLoc);
}

