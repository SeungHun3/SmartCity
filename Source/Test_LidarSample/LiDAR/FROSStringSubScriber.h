// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "ROSBridgeSubscriber.h"
#include "std_msgs/String.h"
#include "Lidar_Basic.h"
#include "Core.h"

class FROSStringSubScriber : public FROSBridgeSubscriber
{
public:
	FROSStringSubScriber(const FString& InTopic);
	~FROSStringSubScriber() override;

	// 제이슨 데이터 분할
	void ParseMessageToJson(TSharedPtr<FJsonObject> jdata);

	TSharedPtr<FROSBridgeMsg> ParseMessage(TSharedPtr<FJsonObject> JsonObject) const override;

	void Callback(TSharedPtr<FROSBridgeMsg> InMsg) override;

public:
	Lidar_Basic * m_Basic;
};
