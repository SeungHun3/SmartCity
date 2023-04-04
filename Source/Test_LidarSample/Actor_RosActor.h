// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ROSBridgeHandler.h"
#include "LiDAR/Lidar_Basic.h"
#include "Actor_RosActor.generated.h"

UCLASS()
class TEST_LIDARSAMPLE_API AActor_RosActor : public AActor, public Lidar_Basic
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_RosActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString connectIP = FString("183.105.113.142");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 connectPort = 9090;

	// 스폰 플레이어 캐릭터 클래스 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class APawn_LidarPlayer> SpawnPlayer;
	
	// 라이다 스폰 캐릭터  // 라이다 접속 데이터  TArray<FLidarData> ArrayPlayerLidarData;
	TArray<class APawn_LidarPlayer*> ArrayPlayers;
	
	TSharedPtr<FROSBridgeHandler> ROSBridgeHandler;
	TSharedPtr<FROSBridgePublisher> Publisher; 

	// 라이다 접속 중 ID
	TArray<int> CheckUserID;
	TArray<int> CheckRemoveID;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
		void CheckTimer();
	float CurrentTimer;
	FTimerHandle CheckTimerHandle;

	void setPlayerDataAverage();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ( 라이다 ) 데이터 처리 Lidar_Basic
	void RemoveLidarData(int id);

	// ( 라이다 ) 데이터 처리 Lidar_Basic
	virtual void updateLidar(FLidarData data) override;

	// ( 라이다 ) 게임 데이터 제거
	virtual void RemovePlayerData() override;

	// 게임 데이터 스폰 
	void AddPlayerData(FLidarData data);

	// 게임 데이터 변경
	void updatePlayerData(int PlayerId);
};
