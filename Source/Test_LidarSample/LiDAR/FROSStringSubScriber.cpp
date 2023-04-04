// Fill out your copyright notice in the Description page of Project Settings.


#include "FROSStringSubScriber.h"
#include "../GameInstance_Lidar.h"


FROSStringSubScriber::FROSStringSubScriber(const FString& InTopic) :
    FROSBridgeSubscriber(InTopic, TEXT("std_msgs/String"))
{
	

}

FROSStringSubScriber::~FROSStringSubScriber()
{


};

// 라이다 데이터 분할 & 구조 설계
void FROSStringSubScriber::ParseMessageToJson(TSharedPtr<FJsonObject> jdata) 
{
	TArray<TSharedPtr<FJsonValue>> JsonValueArray = jdata->GetArrayField(TEXT("trkobjs"));

	for (auto it : JsonValueArray)
	{
		TSharedPtr<FJsonObject> JsonObject = it->AsObject();

		FLidarData WebLidarData;
		//	"heading" : 0.024018211093159818,
		//	"id" : 32746,
		//	"lwh" : [0.46954536437988281, 0.39302074909210205, 1.2616817653179169] ,
		//	"pos" : [14.045886993408203, 1.0199435949325562, 0.86719024181365967] ,
		//	"tracklet" : [
		//		[13.734832347156855, 1.0071724660626646, 0.8692394823148506],
		//			[13.63790814435832, 1.00236215250966, 0.8682271004147335],
		//			[13.520701903704042, 0.99921165074577789, 0.87230110471615863],
		//			[13.407287589795727, 0.99650545921087508, 0.88109271235903563],
		//			[11.239120634667215, 2.4559955578040196, 0.80824153042888647],
		//			[11.263564633983606, 2.4166880785815912, 0.84362742403056767],
		//			[11.284614492613249, 2.3564532998780745, 0.83455620360634197]
		//	],
		//	"type": "ped",
		//	"vel" : [1.1143183633682385, 0.026769081340820844, -0.042193211732357572]
		//  "zone_num": [1]

		WebLidarData.ID = FCString::Atoi(*JsonObject->GetStringField(TEXT("id")));
		
		// 물체 회전량
		FString strHeading = JsonObject->GetStringField(TEXT("heading"));
		WebLidarData.heading = FCString::Atof(*strHeading);

		// vel // 물체 속도 
		TArray<TSharedPtr<FJsonValue>> velArray = JsonObject->GetArrayField(TEXT("vel"));
		for (auto it1 : velArray)
		{
			uint64 vVel;
			if (it1->TryGetNumber(vVel))
			{
				WebLidarData.Vel.Add(it1->AsNumber());
			}
		}

		// type // 물체 정보 타입
		WebLidarData.Type = JsonObject->GetStringField(TEXT("type"));

		// lwh // 가로 세로 높이 double
		TArray<TSharedPtr<FJsonValue>> lwhArray = JsonObject->GetArrayField(TEXT("lwh"));
		for (auto it2 : lwhArray)
		{
			WebLidarData.lwh.Add(it2->AsNumber());
		}

		// pos // 위치 정보 
		TArray<TSharedPtr<FJsonValue>> PosArray = JsonObject->GetArrayField(TEXT("pos"));
		for (auto it3 : PosArray)
		{
			WebLidarData.pos.Add(it3->AsNumber());
		}

		// 존 , 필드 정보
		WebLidarData.zone_num.Reset();
		TArray<TSharedPtr<FJsonValue>> ZoneArray = JsonObject->GetArrayField(TEXT("zone_num"));
		for (auto it4 : ZoneArray)
		{
			WebLidarData.zone_num.Add(*it4->AsString());
		}
		
		// checkf(m_Basic, TEXT("// Lidar Basic NULL"))
		m_Basic->updateLidar(WebLidarData);
	}
	// 
	m_Basic->RemovePlayerData();
}

// ##### Destructor
// Class destructors will be required if you need to do some cleaning work after the ROS bridge client disconnects.

TSharedPtr<FROSBridgeMsg> FROSStringSubScriber::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const 
{
	TSharedPtr<std_msgs::String> StringMessage = MakeShareable<std_msgs::String>(new std_msgs::String());

	StringMessage->FromJson(JsonObject);
	
    return StaticCastSharedPtr<FROSBridgeMsg>(StringMessage);
}


void FROSStringSubScriber::Callback(TSharedPtr<FROSBridgeMsg> InMsg)
{
	TSharedPtr<std_msgs::String> StringMessage = StaticCastSharedPtr<std_msgs::String>(InMsg);
	
	TMap<FString, TSharedPtr<FJsonValue>> ParseJonValue = StringMessage->ToJsonObject()->Values;
	for (auto it : ParseJonValue)
	{
		switch (it.Value->Type)
		{
			case EJson::String:
			{
				TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(*it.Value->AsString());
				TSharedPtr<FJsonObject> ReaderJsonObject = MakeShareable(new FJsonObject());
				if (FJsonSerializer::Deserialize(Reader, ReaderJsonObject))
				{
					ParseMessageToJson(ReaderJsonObject);
				}
			}
		}
	}
	return;
}