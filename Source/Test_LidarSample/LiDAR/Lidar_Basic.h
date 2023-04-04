// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GameInstance_Lidar.h"


class TEST_LIDARSAMPLE_API Lidar_Basic
{
public:
	Lidar_Basic();
	virtual ~Lidar_Basic();

	virtual void updateLidar(FLidarData webdata) {}
	virtual void RemovePlayerData() {}
};
