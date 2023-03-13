// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstance_Lidar.generated.h"


USTRUCT(BlueprintType)
struct FLidarData
{
	GENERATED_BODY()

public:
	//- id : 객체 번호
	UPROPERTY(BlueprintReadWrite)
		int ID;
	//- type : 객체 타입(사람, 자동차, …)
	UPROPERTY(BlueprintReadWrite)
		FString Type;
	//- pos : 객체 위치(x, y, z) m
	UPROPERTY(BlueprintReadWrite)
		TArray<float> pos;
	//- vel : 객체 속도(m / s)
	UPROPERTY(BlueprintReadWrite)
		TArray<float> Vel;
	//- lwh : 객체 길이, 넓이, 높이(m)
	UPROPERTY(BlueprintReadWrite)
		TArray<float> lwh;
	//- heading : 객체 이동 방향(radian)
	UPROPERTY(BlueprintReadWrite)
		float heading;
	//- tracklet : 객체의 이동 궤적
	UPROPERTY(BlueprintReadWrite)
		TArray<FVector> tracklet;
	//- zone_num : 현재 지정 필드 상호작용 여부
	UPROPERTY(BlueprintReadWrite)
		TArray<FString> zone_num;

public:
	FVector getPosition() {
		if (pos.Num() == 0)
			return FVector::ZeroVector;
		return FVector(pos[0], pos[1] * -1.f, pos[2]) * 100.f;
	}
	FVector getVelocity() {
		FVector vVelocity = FVector::ZeroVector;
		for (uint8 it = 0; it < Vel.Num(); it++)
		{
			if(it == 0)
				vVelocity.X = Vel[it];
			else if (it == 1)
				vVelocity.Y = Vel[it];
			else if (it == 2)
				vVelocity.Z = Vel[it];
		}
		return vVelocity;
	}
	FVector getScale() {
		if (lwh.Num() == 0)
			return FVector::ZeroVector;
		return FVector(lwh[0], lwh[1], lwh[2]);
	}
	float getHeading() {
		return heading * 180.f / 3.14f * -1.f;
	}
};

//UENUM(BlueprintType)
//enum class ELidarType : uint8
//{
//
//};

UCLASS()
class TEST_LIDARSAMPLE_API UGameInstance_Lidar : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FLidarData> ArrayWebLidarData;
	UPROPERTY()
		TArray<int> ChangeLidarDataID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ConnectServerURL = FString("ws://183.105.113.142:9090"); // FString("ws://localhost:8000/");  // 

private:


public:

	virtual void Init() override;

	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
		void ConnectedWebSocket();
	UFUNCTION()
		void getLidarPlayerData();
	UFUNCTION()
		void AddLidarPlayerData(FLidarData data);
	UFUNCTION()
		void RemoveLidarPlayerData();
	// Reader 
	void readLidarJsonData(const FString& jsonstr);

	//// 웹 소켓 연결 
	//UFUNCTION()
	//	void OnConnectedWeb();
	//UFUNCTION()
	//	void OnConnectedWebError();
	//UFUNCTION()
	//	void OnClosedWeb();
	//UFUNCTION()
	//	void OnMessage();
	//UFUNCTION()
	//	void OnRowMessage();
};
