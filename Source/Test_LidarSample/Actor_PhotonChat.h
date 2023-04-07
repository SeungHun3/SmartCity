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
	Public, // �Ϲ� ä�ε�
	Channel,
	Private, // �ӼӸ�.
	TEST,
	Max,
};


UCLASS()
class TEST_LIDARSAMPLE_API AActor_PhotonChat : public AActor , public SH_PhotonChatBasic
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActor_PhotonChat();

	// ä�� ä�� ����Ʈ 
	TArray<FString> SubscribeList;

	// ���� ä�� ä��
	FString targetSubscribe = FString("TEST");

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Message 
	UFUNCTION(BlueprintCallable)
		void SendMessageEvent(const FString& Message);
	UFUNCTION(BlueprintCallable)
		void SendPrivateMessage(const FString& username, const FString& message);
	// set Message Subscribe
	UFUNCTION(BlueprintCallable)
		void setTargetSubscribe(enum_PhotonChat target); // targetSubscribe
	
	// Subscribe ä�� �߰�
	UFUNCTION(BlueprintCallable)
		void AddSubscribeEvent(TArray<FString> sub);

	// �÷����� �α��� ������ ȣ�� �Ͽ� id ����ȭ
	UFUNCTION(BlueprintCallable)
		void Connect_Chat(const FString& playfabid);

	////////////////////////////////////////////////////////////////////////////
	// Basic Class override / SH_PhotonChatListener
	
	// ���� ê ���� ���� 
	void Chat_ConnectComplete(void) override;
	// ���� ä�� �߰� 
	void Chat_AddSubscribe(const FString& Channel);
	// ���� ä�� ����
	void Chat_RemoveSubscribe(const FString& Channel);

	////////////////////////////////////////////////////////////////////////////
	// Blueprint Binding Func
	UFUNCTION(BlueprintImplementableEvent)
		void Blueprint_ConnectComplete();
	
	
public:
	class SH_PhotonChatListener* m_pListener;

	ExitGames::Chat::Client* m_pChatClient;

};
