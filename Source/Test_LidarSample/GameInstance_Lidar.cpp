// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance_Lidar.h"

#include "Dom/JsonObject.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"

void UGameInstance_Lidar::Init()
{
	Super::Init();

	// Test
	// getLidarPlayerData();
	// 소컷 서버 연결
	// ConnectedWebSocket();
}

void UGameInstance_Lidar::Shutdown() 
{
	Super::Shutdown();

}

// 웹 소켓 연결 받기 
void UGameInstance_Lidar::ConnectedWebSocket()
{
	// const FString ServerURL = TEXT("ws://183.105.113.142:9090"); 
	// Your server URL. You can use ws, wss or wss+insecure.
	const FString ServerURL = ConnectServerURL;			// Your server URL. You can use ws, wss or wss+insecure.
	const FString ServerProtocol = TEXT("ws");			// The WebServer protocol you want to use.

	// UE_LOG(LogTemp, Log, TEXT("// ConnectServerURL :: %s "), *ConnectServerURL);

	TSharedPtr<IWebSocket> Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);


	// We bind all available events
	Socket->OnConnected().AddLambda([]() -> void {
		// This code will run once connected.
		// UE_LOG(LogTemp, Log, TEXT("// OnConnected :: "));
		});

	Socket->OnConnectionError().AddLambda([](const FString& Error) -> void {
		// This code will run if the connection failed. Check Error to see what happened.
		// UE_LOG(LogTemp, Log, TEXT("// OnConnectionError :: %s "), *Error);
		});

	Socket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) -> void {
		// This code will run when the connection to the server has been terminated.
		// Because of an error or a call to Socket->Close().

		// UE_LOG(LogTemp, Log, TEXT("// OnClosed :: code( %d )"), StatusCode);
		});


	Socket->OnMessageSent().AddLambda([](const FString& MessageString) -> void {
		// This code is called after we sent a message to the server.
		// UE_LOG(LogTemp, Log, TEXT("// OnMessageSent :: %s "), *MessageString);
		});

	// 콜 메세지 확인
	Socket->OnMessage().AddLambda([](const FString& Message) -> void {
		// This code will run when we receive a string message from the server.
		// UE_LOG(LogTemp, Log, TEXT("// OnMessage :: "));
		// UE_LOG(LogTemp, Log, TEXT("//		:: %s "), *Message);
		});

	Socket->OnRawMessage().AddLambda([](const void* Data, SIZE_T Size, SIZE_T BytesRemaining) -> void {
		// This code will run when we receive a raw (binary) message from the server.
		// UE_LOG(LogTemp, Log, TEXT("// OnRowMessage :: Binary "));
		});

	// And we finally connect to the server. 
	Socket->Connect();
}
// Reader Json
void UGameInstance_Lidar::readLidarJsonData(const FString& jsonstr)
{
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(jsonstr);
	TSharedPtr<FJsonObject> jsonObj = MakeShareable(new FJsonObject());

	if (FJsonSerializer::Deserialize(reader, jsonObj) && jsonObj.IsValid())
	{
		// UE_LOG(LogTemp, Log, TEXT("// Deserialize ::"));

		TArray<TSharedPtr<FJsonValue>> JsonValueArray = jsonObj->GetArrayField(TEXT("trkobjs"));

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
			//	"vel" : [1.1143183633682385, 0.026769081340820844, -0.042193211732357572] ,

			WebLidarData.heading = FCString::Atoi(*JsonObject->GetStringField(TEXT("heading")));
			WebLidarData.ID = FCString::Atoi(*JsonObject->GetStringField(TEXT("id")));

			// vel // 물체 속도 
			TArray<TSharedPtr<FJsonValue>> velArray = JsonObject->GetArrayField(TEXT("vel"));
			for (auto it1 : velArray)
			{
				WebLidarData.Vel.Add(it1->AsNumber());
			}

			// type // 물체 정보 타입
			WebLidarData.Type = JsonObject->GetStringField(TEXT("type"));

			// lwh // 가로 세로 높이
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

			// tracklet // 이전 위치 정보 
			//TArray<TSharedPtr<FJsonValue>> trackletArray = JsonObject->GetArrayField(TEXT("tracklet"));
			//for (auto it4 : trackletArray)
			//{
			//	TArray<TSharedPtr<FJsonValue>> trackletArray = JsonObject->GetArrayField(TEXT("tracklet"));
			//	UE_LOG(LogTemp, Log, TEXT("// trackletArray :: %f"), it4->AsNumber());
			//	// WebLidarData.pos.Add(it4->AsNumber());
			//}

			// 데이터 추가 // 동일한 ID or 새로운 데이터 
			// ArrayWebLidarData.Add(WebLidarData);
			AddLidarPlayerData(WebLidarData);
		}
	}
	else
	{
		// UE_LOG(LogTemp, Log, TEXT("// NULL Deserialize ::"));
	}
}
// 체크 데이터 파일
void UGameInstance_Lidar::getLidarPlayerData()
{
	FString filePath = FPaths::ProjectDir() + "simple_test" + "/trkobjs.json";

	// UE_LOG(LogTemp, Log, TEXT("// Project Conten Dir :: %s "), *filePath);

	FString jsonstr;	// Recive load json text
	FFileHelper::LoadFileToString(jsonstr, *filePath);	// Load json to filePath

	// 웹 데이터 제이슨 읽기
	readLidarJsonData(jsonstr);
}
// 라이다 데이터 플레이어 추가 
void UGameInstance_Lidar::AddLidarPlayerData(FLidarData data)
{
	// 변경된 데이터 ID 리스트
	ChangeLidarDataID.Add(data.ID);

	//UE_LOG(LogTemp, Log, TEXT("// AddLidarPlayerData :: ID( %d )"), data.ID);

	//UE_LOG(LogTemp, Log, TEXT("//		heading( %f )"), data.heading);

	//UE_LOG(LogTemp, Log, TEXT("//		lwh.L( %f )"), data.lwh[0]);
	//UE_LOG(LogTemp, Log, TEXT("//		lwh.W( %f )"), data.lwh[1]);
	//UE_LOG(LogTemp, Log, TEXT("//		lwh.H( %f )"), data.lwh[2]);

	//UE_LOG(LogTemp, Log, TEXT("//		Position( %f )"), data.pos[0]);
	//UE_LOG(LogTemp, Log, TEXT("//		Position( %f )"), data.pos[1]);
	//UE_LOG(LogTemp, Log, TEXT("//		Position( %f )"), data.pos[2]);

	//UE_LOG(LogTemp, Log, TEXT("//		Position( %f )"), data.pos[0]);
	//UE_LOG(LogTemp, Log, TEXT("//		Position( %f )"), data.pos[1]);
	//UE_LOG(LogTemp, Log, TEXT("//		Position( %f )"), data.pos[2]);

	//UE_LOG(LogTemp, Log, TEXT("//		Type( %s )"), *data.Type);

	// 2.6 솔라시도 실 데이터 확인 
	// 구분 타입 추가 :: Person , Car 
	for (auto it : ArrayWebLidarData)
	{
		if (it.ID == data.ID)
		{
			it = data;
			return;
		}
	}
	// 찾는 ID 없다면 추가
	ArrayWebLidarData.Add(data);
}
// 라이다 데이터 플레이어 제거
void UGameInstance_Lidar::RemoveLidarPlayerData()
{
	// UE_LOG(LogTemp, Log, TEXT("// Remove Player Data :: ID ( %d )"), );
	// 
	for (int i = 0; i < ArrayWebLidarData.Num(); i++)
	{
		for (auto it2 : ChangeLidarDataID)
		{
			if (ArrayWebLidarData[i].ID == it2)
			{
				break;
			}
		}
		// 업데이트 데이터 없음 // 삭제

		/* Actor 삭제 , 리스트(위젯) 추가 하기 */
		ArrayWebLidarData.RemoveAt(i);

		ChangeLidarDataID.Reset();
		return;
	}
}