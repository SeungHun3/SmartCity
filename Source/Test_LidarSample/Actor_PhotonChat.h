// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Photon/SH_PhotonChatListener.h"
#include "../Photon/Chat-cpp/inc/Client.h"
#include "Actor_PhotonChat.generated.h"

UENUM(BlueprintType)
enum class enum_PhotonChat : uint8
{
	Public, // 일반 채널들
	Channel,
	Private, // 귓속말.
	TEST,
	Max,
	World,
};


UCLASS()
class TEST_LIDARSAMPLE_API AActor_PhotonChat : public AActor , public SH_PhotonChatBasic
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_PhotonChat();

	// 채팅 채널 리스트 
	TArray<FString> SubscribeList;

	// 현재 채팅 채널
	FString targetSubscribe;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Test
	UFUNCTION(BlueprintCallable)
	TArray<FString> getSubscribeList() {
		return SubscribeList;
	}
	// Message 
	UFUNCTION(BlueprintCallable)
		void SendMessageEvent(const FString& Message);
	UFUNCTION(BlueprintCallable)
		void SendPrivateMessage(const FString& username, const FString& message);
	// set Message Subscribe
	UFUNCTION(BlueprintCallable)
		void setTargetSubscribe(enum_PhotonChat target); // targetSubscribe
	
	// Subscribe 채널 추가
	UFUNCTION(BlueprintCallable)
		void AddSubscribeEvent(TArray<FString> sub);

	// 플레이팹 로그인 성공시 호출 하여 id 동기화
	UFUNCTION(BlueprintCallable)
		void Connect_Chat(const FString& playfabid);

	////////////////////////////////////////////////////////////////////////////
	// Basic Class override / SH_PhotonChatListener
	
	// 포톤 챗 접속 성공 
	void Chat_ConnectComplete(void) override;
	void Chat_getMessageEvent(const FString& ChannelName, const FString& sender, const FString& Message) override;

	// 구독 채널 추가 
	void Chat_AddSubscribe(const FString& Channel) override;
	// 구독 채널 제거
	void Chat_RemoveSubscribe(const FString& Channel) override;

	////////////////////////////////////////////////////////////////////////////
	// Blueprint Binding Func
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_ConnectComplete(); // 보이스 스폰액터
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_getMessageEvent(const FString& ChannelName, const FString& Sender, const FString& Message);
public:
	class SH_PhotonChatListener* m_pListener;

	ExitGames::Chat::Client* m_pChatClient;

	FString m_RoomName;
	void Chat_ResetJoinChannel(const FString& FullName);
};
