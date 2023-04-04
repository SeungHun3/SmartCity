// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ROSBridgeSrvClient.h"
#include "rospy_tutorials/AddTwoInts.h"

class FROSAddTwoIntsClient final : public FROSBridgeSrvClient
{
public:
    FROSAddTwoIntsClient(const FString& InName) :
        FROSBridgeSrvClient(InName, TEXT("std_msgs/String")) // TEXT("rospy_tutorials/AddTwoInts"))
    {
    }

    void Callback(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse) override
    {
        // test
        //TSharedPtr<rospy_tutorials::AddTwoInts::Response> Response =
        //    StaticCastSharedPtr<rospy_tutorials::AddTwoInts::Response>(InResponse);

        UE_LOG(LogTemp, Log, TEXT("// FROSAddTwoIntsClient : public FROSBridgeSrvClient"));
        UE_LOG(LogTemp, Log, TEXT("//      %s "), *InResponse->ToString());

    }
};
