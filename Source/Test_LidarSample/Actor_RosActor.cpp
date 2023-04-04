// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_RosActor.h"
#include "Pawn_LidarPlayer.h"
#include "Character_Player.h"

#include "LiDAR/FROSStringSubScriber.h"
#include "LiDAR/FROSAddTwoIntsClient.h"

// Sets default values
AActor_RosActor::AActor_RosActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Blueprint'/Game/Project/Pawn/MyPawn_LidarPlayer.MyPawn_LidarPlayer'
	static ConstructorHelpers::FClassFinder<APawn_LidarPlayer> FindLiDARPlayer(TEXT("/Game/Project/Pawn/MyPawn_LidarPlayer"));
	if (FindLiDARPlayer.Succeeded())
	{
		SpawnPlayer = FindLiDARPlayer.Class;
	}
}

// Called when the game starts or when spawned
void AActor_RosActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Set websocket server address to ws://127.0.0.1:9001 
	// ws://183.105.113.142:9090
	FWebsocketInfoCallBack infoCallBack_Error;
	FWebsocketInfoCallBack infoCallBack_Connected;
	ROSBridgeHandler = MakeShareable<FROSBridgeHandler>(new FROSBridgeHandler(connectIP, connectPort, infoCallBack_Error, infoCallBack_Connected));

	// 연결 에러 출력 
	infoCallBack_Error.AddLambda([&]() {
		UE_LOG(LogTemp , Log , TEXT("// infoCallBack_Error :: "));
		});
	// 연결 성공
	infoCallBack_Connected.AddLambda([&]() {
		UE_LOG(LogTemp, Log, TEXT("// infoCallBack_Connected :: "));
		});

	// **** Create publishers here ****
	// Publisher = MakeShareable<FROSBridgePublisher>(new FROSBridgePublisher(TEXT("/talker"), TEXT("sensor_msgs/JointState")));
	Publisher = MakeShareable<FROSBridgePublisher>(new FROSBridgePublisher(TEXT("/Trkobjs"), TEXT("std_msgs/String")));
	ROSBridgeHandler->AddPublisher(Publisher);

	// Add topic subscribers and publishers
	/*	This plugin uses `FROSBridgeSubscriber` class interface to subscribe to topics. 
		We need to extend a `FROSBridgeSubscriber` subclass for each topic we would like to subscribe to,
		implementing the constructor, destructor, `ParseMessage` function and `Callback` function.*/ 

	/*	이 플러그인은 'FROSBridgeSubscriber' 클래스 인터페이스를 사용하여 주제를 구독합니다. 
		구독하려는 각 주제에 대해 'FROSBridgeSubscriber' 하위 클래스를 확장하여 
		생성자, 소멸자, 'ParseMessage' 함수 및 'Callback' 함수를 구현해야 합니다.*/ 

	// (TEXT("/Trkobjs"), TEXT("std_msgs/String"))); // roslib

	TSharedPtr<FROSStringSubScriber> Subscriber = MakeShareable<FROSStringSubScriber>(new FROSStringSubScriber(TEXT("/Trkobjs"))); // (new FROSStringSubScriber(TEXT("/chatter")));
	Subscriber->m_Basic = this;
	ROSBridgeHandler->AddSubscriber(Subscriber);
	
	// 연결 테스트 
	// TSharedPtr<FROSAddTwoIntsClient> ServiceClient = MakeShareable<FROSAddTwoIntsClient>(new FROSAddTwoIntsClient(TEXT("/Trkobjs"))); //(new FROSAddTwoIntsClient(TEXT("add_two_ints")));

	// ROSBridgeHandler->CallService();
	//int NumA = FMath::RandRange(1, 10000);
	//int NumB = FMath::RandRange(1, 10000);
	//TSharedPtr<FROSBridgeSrv::SrvRequest> Request = MakeShareable(new rospy_tutorials::AddTwoInts::Request(NumA, NumB));
	//// Create a request instance with request parameters
	//TSharedPtr<FROSBridgeSrv::SrvResponse> Response = MakeShareable(new rospy_tutorials::AddTwoInts::Response());
	
	// Create an empty response instance
	// ROSBridgeHandler->CallService(ServiceClient, Request, Response);

	// Connect to ROSBridge Websocket server.

	ROSBridgeHandler->Connect();
	GetWorld()->GetTimerManager().SetTimer(CheckTimerHandle, this, &AActor_RosActor::CheckTimer, 0.01, true);
}

void AActor_RosActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ROSBridgeHandler->Disconnect();
	// UE_LOG(LogTemp, Log, TEXT("// (LIDAR)	EndPlay Disconnect ::  "));
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AActor_RosActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ROSBridgeHandler)
	{
		ROSBridgeHandler->Process();
	}

	static float dTime =+ DeltaTime;
	if (DeltaTime > 1.f)
	{
		// UE_LOG(LogTemp, Log, TEXT("// Roslib Connect False "));
		if (!ROSBridgeHandler->IsClientConnected())
		{
			ROSBridgeHandler->Connect();
			dTime = 0.f;
		}
	}
}

// ( WebLidar ) 데이터 처리
void AActor_RosActor::RemoveLidarData(int id)
{
	for (auto it : CheckUserID)
	{
		// UE_LOG(LogTemp, Log, TEXT("// (LIDAR) Remove PlayerNr :: (Len : { %d }) %d "), ArrayPlayers.Num(), id);
		if (ArrayPlayers.IsValidIndex(id))
		{
			if (ArrayPlayers[id]->PlayerID == it)
			return;
		}
		else
		{
			// UE_LOG(LogTemp, Warning, TEXT("// (LIDAR) Remove Player Data :: NULL "));
			return;
		}
	}
	// 삭제
	if (ArrayPlayers.IsValidIndex(id))
	{
		// ArrayPlayers[id]->Destroy();
		// 오파시티 효과 후 제거로 변경 // 블루프린트 타이머 완료 종료
		ArrayPlayers[id]->setOpacityVIsibility(false);
		ArrayPlayers.RemoveAt(id);
	}
}

// ( LiDAR ) 데이터 처리 딜레이 타임 
void AActor_RosActor::CheckTimer()
{
	CurrentTimer += 0.01f;
}

// ( WebLidar ) 데이터 처리
void AActor_RosActor::updateLidar(FLidarData data)
{
	CheckUserID.Add(data.ID);

	for (int i = 0; i < ArrayPlayers.Num(); i++)
	{
		if (ArrayPlayers[i]->PlayerID == data.ID)
		{
			//UE_LOG(LogTemp, Log, TEXT("// (LIDAR) update Player Data	:: %d "), ArrayPlayers[i]->PlayerID);

			// 데이터 출력 시간 체크
			CurrentTimer = 0.f;
			
			// 데이터 NPC 캐릭터 적용
			// ArrayPlayers[i]->ChangeScaleObject(data.getScale());
			// ArrayPlayers[i]->ChangeVelocity(data.getVelocity());
			// ArrayPlayers[i]->ChangeVelocityDist(FVector::Dist(FVector::ZeroVector, data.getVelocity()));

			ArrayPlayers[i]->setActorVelocity(data.getVelocity());
			ArrayPlayers[i]->setActorLocationLerp(data.getPosition());

			// UE_LOG(LogTemp, Log, TEXT("// (LIDAR) update setActorRotationLerp :: %f "), data.heading);
			ArrayPlayers[i]->setActorRotationLerp(data.getHeading());
			
			// ArrayPlayers[i]->SetActorRotation(FRotator(0, data.getHeading(), 0));

			// 캐릭터 높이 값에 따른 타입 구분 // Adult , Baby
			ArrayPlayers[i]->setPlayerTypeLiDAR(data.Type);
			
			// 현재 등록된 지역 정보 데이터 // 중복 지역 x
			if (data.zone_num.Num() > 0)
				ArrayPlayers[i]->setZoneNum(data.zone_num[0]);
			else
				ArrayPlayers[i]->setZoneNum(FString("0"));

			// 라이다 캐릭터 데이터 수치 
			// 킨텍스 UI 전용
			setPlayerDataAverage();
			return;
		}
	}
	AddPlayerData(data);	
}
// LiDAR NPC 데이터 스폰
void AActor_RosActor::AddPlayerData(FLidarData data)
{
	// LiDAR Character 스폰
	// UE_LOG(LogTemp, Log, TEXT("// (LIDAR) ADD Player Data :: %d "), data.ID);
	
	// 2.6 신규 데이터 타입 확인 추가
	if (data.Type == FString("Person") || data.Type == FString("Car"))
	{
		FActorSpawnParameters params = FActorSpawnParameters();
		FVector spawnLocation = FVector();
		FRotator rot = FRotator();
		FTransform asd = FTransform();

		APawn_LidarPlayer* player = GetWorld()->SpawnActor<APawn_LidarPlayer>(SpawnPlayer, asd, FActorSpawnParameters());
	
		if (player)
		{
			player->PlayerID = data.ID;
			player->setNameTag(FString(data.Type) + FString::FromInt(data.ID));
			player->setOpacityVIsibility(true);
			ArrayPlayers.Add(player);
		}
	}
}

// 데이터 출력 변경
void AActor_RosActor::updatePlayerData(int PlayerId)
{
	// ArrayPlayerLidarData[PlayerId];
	// UE_LOG(LogTemp, Log, TEXT("// (LIDAR) update Player Data	:: %d "), ArrayPlayers[PlayerId]->PlayerID);
	// UE_LOG(LogTemp, Log, TEXT("//			Position Data		:: %s "), *ArrayPlayerLidarData[PlayerId].getPosition().ToString());

}

// ( WebLidar ) 데이터 처리 캐릭터 삭제
void AActor_RosActor::RemovePlayerData()
{
	TArray<APawn_LidarPlayer*> TempLidarData = ArrayPlayers;
	for (int i = 0; i < TempLidarData.Num(); i++)
	{
		RemoveLidarData(i);
	}

	CheckUserID.Reset();
}

// ( LIDAR ) 평균 데이터 처리 
void AActor_RosActor::setPlayerDataAverage()
{
	float avgVelocity = 0.f;
	float avgHeight = 0.f;;

	float avgVelocityX = 0.f;
	float avgVelocityY = 0.f;
	// vel			2.0 ~ 0.9 ~ 0.2 // 1.8	// 1.8 ~ 0.7 ~ 0 
	// // vel X		
	// // vel Y		
	// height		230 ~ 180 ~ 130 // 100	// 100 ~ 50 ~ 0	

	for (auto it : ArrayPlayers)
	{
		avgVelocity += it->MoveVelocity.Size();
		avgHeight += it->NextLoc.Z;

		avgVelocityX += it->MoveVelocity.X;
		avgVelocityY += it->MoveVelocity.Y;
	}

	avgVelocity /= ArrayPlayers.Num();
	avgHeight /= ArrayPlayers.Num();

	// UE_LOG(LogTemp, Log, TEXT("// (LIDAR) :: avgVelocity ( %f ) , avgHeight ( %f )"), avgVelocity, avgHeight * 2);

	ACharacter_Player* LocalPlayer = Cast<ACharacter_Player>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if(LocalPlayer)
	{
		LocalPlayer->setLiDARinformation((avgVelocity - 0.2f) / 1.8, (avgHeight * 2.f - 130.f) / 100);
		LocalPlayer->setLiDARVelocity(avgVelocityX, avgVelocityY);
	}
}