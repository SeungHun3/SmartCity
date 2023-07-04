// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_PhotonChat.h"

#define CHAT_AppID "bd96a445-11ab-4bc6-bb45-71b681b17273"
#define WorldChannel "World"
using namespace ExitGames;

// Sets default values
AActor_PhotonChat::AActor_PhotonChat()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pListener = new SH_PhotonChatListener(this);
	m_pChatClient = new ExitGames::Chat::Client(*m_pListener, Common::JString(CHAT_AppID), Common::JString("ns.exitgames.com"));

	m_pListener->setChatClient(m_pChatClient);
}

void AActor_PhotonChat::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (m_pListener)
		delete m_pListener;
	if (m_pChatClient)
		delete m_pChatClient;
}
// Called when the game starts or when spawned
void AActor_PhotonChat::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void AActor_PhotonChat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_pChatClient)
		m_pChatClient->service();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// connect
// check playfab login id 
void AActor_PhotonChat::Connect_Chat(const FString& playfabid)
{
	// 테스트 접속
	FString UserID = playfabid;
	if (m_pChatClient)
	{
		ExitGames::Chat::AuthenticationValues AuthenticationValue;
		AuthenticationValue.setUserID(TCHAR_TO_UTF8(*UserID));
		
		m_pChatClient->connect(AuthenticationValue);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Message
// 채널 추가하기
void AActor_PhotonChat::AddSubscribeEvent(TArray<FString> sub)
{
	Common::JVector<Common::JString> Subscribechannels;
	for (auto it : sub)
	{
		FString Subscribe = it;
		Subscribechannels.addElement(TCHAR_TO_UTF8(*Subscribe));
	}
	m_pChatClient->opSubscribe(Subscribechannels);
	
}
// 일반 메세지 
void AActor_PhotonChat::SendMessageEvent(const FString& Message)
{
	// Channel Name;
	// 구독 채널 리스트 관리 후 타겟 채널(targetSubscribe)에 메세지 보내기
	
	m_pChatClient->opPublishMessage(TCHAR_TO_UTF8(*targetSubscribe)  , TCHAR_TO_UTF8(*Message));
}
// 귓속말 
void AActor_PhotonChat::SendPrivateMessage(const FString& username, const FString& message)
{
	m_pChatClient->opSendPrivateMessage(TCHAR_TO_UTF8(*username), TCHAR_TO_UTF8(*message));
}
// 채팅 타겟 변경  // 룸 -> 월드 or 귓말 
void AActor_PhotonChat::setTargetSubscribe(enum_PhotonChat target)
{
	if (target == enum_PhotonChat::World)
	{
		targetSubscribe = WorldChannel;
	}
	else {
		targetSubscribe = m_RoomName + "_" + FString::FromInt((int)target);
	}
	// 변경에 따른 UI 수정
	// ChangeTargetSubscribe(target);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic Class override / Binding Event 
void AActor_PhotonChat::Chat_ConnectComplete(void)
{
	
	// Blueprint
	Blueprint_ConnectComplete(); // 보이스채팅 스폰액터
	
	
}
// 구독자 추가 
void AActor_PhotonChat::Chat_AddSubscribe(const FString& Channel)
{
	SubscribeList.AddUnique(Channel);
}
// 구독자 제거 
void AActor_PhotonChat::Chat_RemoveSubscribe(const FString& Channel)
{
	SubscribeList.Remove(Channel);
}

// 채널 접속
void AActor_PhotonChat::Chat_ResetJoinChannel(const FString& FullName)
{// 클라이언트 룸이 있다면 전부 지우고 새로 생성

	//기존리스트 가지고 있다면 삭제 후 초기화
	if (SubscribeList.IsValidIndex(0))
	{
		Common::JVector<Common::JString> Subscribechannels;
		for (auto it : SubscribeList)
		{
			Subscribechannels.addElement(TCHAR_TO_UTF8(*it));
		}
		m_pChatClient->opUnsubscribe(Subscribechannels);
	}
	SubscribeList.Reset();


	m_RoomName = FullName;

	for (int i = 0; i < (int)enum_PhotonChat::Max; i++)
	{
		FString ChannelName = m_RoomName + "_" + FString::FromInt(i); // 일반, 귓속말 ...
		SubscribeList.Add(ChannelName);
	}
	FString WorldName = WorldChannel;
	SubscribeList.Add(WorldName);
	AddSubscribeEvent(SubscribeList);

	// 디폴트 채널 설정
	setTargetSubscribe(enum_PhotonChat::Public);

}
// 메세지 출력 
void AActor_PhotonChat::Chat_getMessageEvent(const FString& ChannelName, const FString& sender, const FString& Message)
{
	// 메세지 위젯 출력
	Blueprint_getMessageEvent(ChannelName, sender, Message);
}