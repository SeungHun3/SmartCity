// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_LidarPlayer.h"
#include "Character_Player.h"

// Sets default values
APawn_LidarPlayer::APawn_LidarPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

}

// Called when the game starts or when spawned
void APawn_LidarPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	player = Cast<ACharacter_Player>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (player)
		player->AddLiDARPlayerCount();
}

void APawn_LidarPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	switch (EndPlayReason)
	{
		case EEndPlayReason::Destroyed:
			{
				// UE_LOG(LogTemp, Log, TEXT("// EndPlay LiDAR Player :: %d "), PlayerID);
				// ACharacter_Player* player = Cast<ACharacter_Player>(GetWorld()->GetFirstPlayerController()->GetPawn());
				if (player)
					player->RemoveLiDARPlayerCount();
				break;
			}
	}
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void APawn_LidarPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APawn_LidarPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 속도 업데이트
void APawn_LidarPlayer::setActorVelocity(FVector vel)
{
	MoveVelocity = vel;
}

// Set LerpLocation :: LiDAR move Location
void APawn_LidarPlayer::setActorLocationLerp(FVector loc)
{
	if (FVector::ZeroVector == loc)
		return;

	GetWorld()->GetTimerManager().ClearTimer(LerpLocationHandle);
	if (NextLoc == FVector::ZeroVector)
		SetActorLocation(loc);

	if (NextLoc == loc)
	{
		bWalk = false;
		return;
	}

	CurrentLoc = GetActorLocation();
	NextLoc = loc;

	GetWorld()->GetTimerManager().SetTimer(LerpLocationHandle, this, &APawn_LidarPlayer::setLocationLerp, 0.01f, true);
}
void APawn_LidarPlayer::setLocationLerp()
{
	bWalk = true;
	FVector Len = (NextLoc - CurrentLoc) * 0.01f;
	FVector LerpLocation = GetActorLocation() + Len;

	// 보간 높이 제외
	SetActorLocation(FVector(LerpLocation.X, LerpLocation.Y, 80.f)); // NextLoc.Z
}

// set Lerp Rotation
void APawn_LidarPlayer::setActorRotationLerp(float fYaw)
{
	if (FMath::Abs(fYaw) > 0.f)
	{
		// UE_LOG(LogTemp, Log, TEXT("// (LiDAR) moveRotation :: %f "), fYaw);
		SetActorRotation(FRotator(0.f, fYaw, 0.f));
	}

	//GetWorld()->GetTimerManager().ClearTimer(LerpRotationHandle);
	//if (NextRot == 0.f)
	//	SetActorRotation(FRotator(0, fYaw, 0));

	//CurrentRot = GetActorRotation().Yaw;
	//NextRot	= fYaw;

	//GetWorld()->GetTimerManager().SetTimer(LerpRotationHandle, this, &APawn_LidarPlayer::setRotationLerp, 0.01f, true);
}
void APawn_LidarPlayer::setRotationLerp()
{
	float fLen = (NextRot - CurrentRot) * 0.1f;
	SetActorRotation(FRotator(0, GetActorRotation().Yaw + fLen, 0));
}

// 라이다 캐릭터 타입 // 어른, 아이 기준
void APawn_LidarPlayer::setPlayerTypeLiDAR(const FString& type)
{
	// Adult, 이하인 객체는 Baby // SK 테스트 버전 타입 Person
	if (type == FString("Adult")) 
	{
		if (TypeLiDAR != enum_TypeLiDAR::Adult)
		{
			TypeLiDAR = enum_TypeLiDAR::Adult;
		}
		else
			return;
	}
	else if (type == FString("Baby"))
	{
		if (TypeLiDAR != enum_TypeLiDAR::Baby)
		{
			TypeLiDAR = enum_TypeLiDAR::Baby;
		}
		else
			return;
	}
	// Blueprint Check
	setCharacterType(TypeLiDAR);
}

// 라이다 설정 구역 오버랩 기준
void APawn_LidarPlayer::setZoneNum(const FString& zone)
{
	if (ZoneNumber != zone)
	{
		ZoneNumber = zone;
		setZoneNumber(ZoneNumber);
	}
}