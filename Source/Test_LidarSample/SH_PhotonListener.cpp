// Fill out your copyright notice in the Description page of Project Settings.


#include "SH_PhotonListener.h"

#ifdef PLATFORM_ANDROID
#include "Engine/Engine.h"
#include "../Photon/LoadBalancing-cpp/inc/ConnectOptions.h"
#endif // PLATRORM_ANDROID

#include <iostream>
#include <stdlib.h>

#ifdef _EG_ANDROID_PLATFORM
#	include <stdarg.h>
#	include <android/log.h>
#endif

using namespace ExitGames::Common;
using namespace ExitGames::LoadBalancing;

SH_PhotonListener::SH_PhotonListener(SH_PhotonBasic* pView)
{
	m_pView = pView;
	m_pClient = NULL;
	LocalPlayerNr = 0;
}

SH_PhotonListener::~SH_PhotonListener(void)
{

}

void SH_PhotonListener::SetClient(ExitGames::LoadBalancing::Client* client)
{
	this->m_pClient = client;
}

void SH_PhotonListener::service()
{
	if (m_pClient)
		m_pClient->service();
}

// 접속
void SH_PhotonListener::Connect(const JString& userName, const JString& serverAddress)
{
	m_pClient->setAutoJoinLobby(true);
	m_pClient->connect(ConnectOptions(AuthenticationValues().setUserID(JString() + GETTIMEMS()), userName, serverAddress));
}
// 지역 선택
void SH_PhotonListener::onAvailableRegions(const JVector<JString>& availableRegions, const JVector<JString>& availableRegionServers)
{
	int size = availableRegions.getSize();
	for (int i = 0; i < size; i++)
	{
		FString str = availableRegions[i].UTF8Representation().cstr();
	}
	m_pView->setRegion();
}
// 연결 종료
void SH_PhotonListener::disconnectReturn(void)
{
	m_pView->InitPlayers();
	m_pView->ErrorCheckMessage("// DisconnectReturn :: ", 99);
	// m_pView->ReconnectMessage();
}
// 접속 성공 
void SH_PhotonListener::connectReturn(int errorCode, const JString& errorString, const JString& region, const JString& cluster)

{
	if (errorCode == ErrorCode::OK)
	{
		FString str = m_pClient->getLocalPlayer().getName().UTF8Representation().cstr();
		
		RoomOptions options;
		options.setMaxPlayers(15);
		
		m_pClient->opJoinOrCreateRoom("LiDARLobby", options);
	}
}
// 방 생성 & 참가 성공
void SH_PhotonListener::joinOrCreateRoomReturn(int localPlayerNr, const Common::Hashtable& roomProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	if (errorCode == ErrorCode::OK)
	{
		int playersize = playerProperties.getSize();
		// UE_LOG(LogTemp, Log, TEXT("// joinOrCreateRoomReturn :: playerProperties.getSize() :: %d"), playersize);

		// this->LocalPlayerNr = localPlayerNr;
		MutableRoom& myRoom = m_pClient->getCurrentlyJoinedRoom();
		Hashtable props = myRoom.getCustomProperties();

		// 접속중인 방 기본 정보
		JString Name = myRoom.getName();
		nByte Count = myRoom.getPlayerCount();
		nByte Maxcount = myRoom.getMaxPlayers();
		m_pView->CurrentRoomInfo(Name, Count, Maxcount);

		// FString str = "Ev";
		// 라이다 이벤트 출력 // 데이터 테이블 quiz Row number
		m_pView->updateRoomProperties(props);

		// 기존 방에 있는 플레이어 정보 출력 
		const JVector<Player*>& players = myRoom.getPlayers();
		int size = players.getSize();
		for (int i = 0; i < size; ++i)
		{
			const Player* p = players[i];
			bool local = p->getNumber() == m_pClient->getLocalPlayer().getNumber();

			// 외형 정보
			Hashtable table = p->getCustomProperties();
			m_pView->AddPlayers(p->getNumber(), p->getName().UTF8Representation().cstr(), local, table);
		}
		m_pView->ConnectComplete();
	}
	else
	{
		FString str = UTF8_TO_TCHAR(errorString.UTF8Representation().cstr());
		// UE_LOG(LogTemp, Log, TEXT("// joinOrCreateRoomReturn Error :: %s , %d"), *str, errorCode);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Custom Event 
void SH_PhotonListener::customEventAction(int playerNr, nByte eventCode, const Object& eventContentObj)
{
	ExitGames::Common::Hashtable eventContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();

	Object const* obj = eventContent.getValue("1");
	if (!obj)	obj = eventContent.getValue((nByte)1);
	if (!obj)	obj = eventContent.getValue(1);
	if (!obj)	obj = eventContent.getValue(1.0);

	if (eventCode == 1) // Chat , World Message 
	{
		if (obj && obj->getDimensions() == 1)
		{
			JString* data = ((ValueObject<JString*>*)obj)->getDataCopy();
			JString Message = data[0];
			JString Type = data[1];

			m_pView->getTextMessage(playerNr, Message, Type);
			return;
		}
	}
	else if (eventCode == 6) // vector3 위치 데이터
	{
		if (obj && obj->getDimensions() == 1) 
		{
			int* data = ((ValueObject<int*>*)obj)->getDataCopy();
			int vX = data[0];
			int vY = data[1];
			int vZ = data[2];
			
			m_pView->GetMovePlayer(playerNr, vX, vY, vZ);
			return;
		}
	}
	else if (eventCode == 7)
	{
		if (obj && obj->getDimensions() == 1) // float 캐릭터 회전 데이터
		{
			if (obj->getType() == TypeCode::EG_FLOAT)
			{
				float* data = ((ValueObject<float*>*)obj)->getDataCopy();
				float fX = data[0];
				m_pView->GetMovePlayerRotation(playerNr, fX);
				return;
			}
		}
	}
	else if (eventCode == 11)
	{
		if (obj && obj->getDimensions() == 1) // 이벤트 일시정지
		{
			if (obj->getType() == TypeCode::BOOLEAN)
			{
				bool* data = ((ValueObject<bool*>*)obj)->getDataCopy();
				bool pause = data[0];
				m_pView->getEventPause(pause);
				return;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// 현재 접속 중인 인원에게 
void SH_PhotonListener::joinRoomEventAction(int playerNr, const JVector<int>& playernrs, const LoadBalancing::Player& player)
{
	// 방 접속시 멀티유저 브로드케스팅 함수
	bool local = player.getNumber() == m_pClient->getLocalPlayer().getNumber();
	if (local)
		return;

	// 외형 정보
	Hashtable table = player.getCustomProperties();
	// UE_LOG(LogTemp, Log, TEXT("// joinRoomEventAction :: %d"), playerNr);
	m_pView->AddPlayers(playerNr, player.getName().UTF8Representation().cstr(), local, table);
}

// 방 떠나기 // 현재 접속중인 방 플레이어 에게 떠난다고 알리기
void SH_PhotonListener::leaveRoomEventAction(int playerNr, bool isInactive)
{
	// UE_LOG(LogTemp, Log, TEXT("// Leave Player Nr :: %d"), playerNr);
	m_pView->RemovePlayer(playerNr);
}

// 접속자 데이터 변경 
void SH_PhotonListener::onPlayerPropertiesChange(int playerNr, const Common::Hashtable& changes)
{
	m_pView->updatePlayerProperties(playerNr, changes);
}

// (LiDAR) 방 정보 변경 // ev , notice
void SH_PhotonListener::onRoomPropertiesChange(const Common::Hashtable& changes)
{
	m_pView->updateRoomProperties(changes);
}

// 캐릭터 움직임, 애니메이션
void SH_PhotonListener::setPlayerAnimationData(uint8 anim)
{
	MutablePlayer& player = m_pClient->getLocalPlayer();
	Hashtable table;

	FString str = "An";
	table.put(TCHAR_TO_UTF8(*str), anim);

	// UE_LOG(LogTemp, Log, TEXT("// Change Animation :: %d"), anim);
	player.addCustomProperties(table);
}

// ( LiDAR ) RoomProperties // 이벤트, 공지메세지 
// ( LiDAR ) 이벤트 정보 출력  // "Ev" >> 이벤트 데이터 테이블 (Data_LiDARQuiz) 행넘버
void SH_PhotonListener::setRoomEventProperties(uint8 Ev)
{
	MutableRoom& CheckRoom = m_pClient->getCurrentlyJoinedRoom();
	Hashtable table;

	FString str = "Ev";
	table.put(TCHAR_TO_UTF8(*str), Ev);
	
	// UE_LOG(LogTemp, Log, TEXT("// Add Room Custom Properties :: %d"), Ev);
	CheckRoom.addCustomProperties(table);
}

// 방 떠나기 성공
void SH_PhotonListener::leaveRoomReturn(int errorCode, const JString& errorString)
{
	if (errorCode == ErrorCode::OK)
	{
		m_pView->LeaveRoomComplete();
		m_pView->InitPlayers();
	}
	else
	{
		// FString str = UTF8_TO_TCHAR(errorString.UTF8Representation().cstr());
		// UE_LOG(LogTemp, Log, TEXT("// LeaveRoom Return Error :: %s , %d"), *str, errorCode);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 위치 전송 // Location & Rotation 
void SH_PhotonListener::SetMovePlayer(int vX, int vY, int vz)
{
	Hashtable HashData;
	int coords[] = { static_cast<int>(vX) , static_cast<int>(vY) ,static_cast<int>(vz) };
	HashData.put((nByte)1, coords, 3);
	m_pClient->opRaiseEvent(false, HashData, 6);
}
// Rotation 
void SH_PhotonListener::SetMovePlayerRotation(float fZ)
{
	Hashtable data;
	float coords[] = { static_cast<float>(fZ) };
	data.put((nByte)1, coords, 1);
	m_pClient->opRaiseEvent(false, data, 7);
}
// 특정 유저에게 보내는 메세지 
void SH_PhotonListener::PlayerTargetMessage(const int* target, int size, nByte text)
{
	Hashtable data;
	nByte coords[] = { static_cast<nByte>(text) };
	data.put((nByte)1, coords, 1);
	RaiseEventOptions option;
	option.setTargetPlayers(target, size);
	m_pClient->opRaiseEvent(false, data, 2, option);
}
// 월드 메시지 
void SH_PhotonListener::TextMessage(const JString& message, const JString& type)
{
	Hashtable data;
	JString coords[] = { static_cast<JString>(message)  , static_cast<JString>(type) };
	data.put((nByte)1, coords, 2);
	RaiseEventOptions Options;
	Options.setReceiverGroup(ExitGames::Lite::ReceiverGroup::ALL);
	m_pClient->opRaiseEvent(false, data, 1, Options);
}
//
void SH_PhotonListener::setEventPause(bool ev)
{
	Hashtable data;
	bool coords[] = { static_cast<bool>(ev) };
	data.put((nByte)1, coords, 1);
	RaiseEventOptions Options;
	Options.setReceiverGroup(ExitGames::Lite::ReceiverGroup::ALL);
	m_pClient->opRaiseEvent(false, data, 11, Options);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 접속 오류 처리 
void SH_PhotonListener::connectionErrorReturn(int errorCode)
{
	m_pView->ErrorCheckMessage("// Connection failed with error", errorCode);
}
/**	서버 에러 문구 처리
	*/
void SH_PhotonListener::debugReturn(int debugLevel, const JString& string)
{
	// Console::get().debugReturn(debugLevel, string);
	FString str = UTF8_TO_TCHAR(string.UTF8Representation().cstr());
	m_pView->ErrorCheckMessage(str, debugLevel);
}

void SH_PhotonListener::clientErrorReturn(int errorCode)
{
	// Console::get().writeLine(JString(L"received error ") + errorCode + L" from client");
	FString str = "// received error :: " + FString::FromInt(errorCode) + " from client";
	m_pView->ErrorCheckMessage(str, errorCode);
}

void SH_PhotonListener::warningReturn(int warningCode)
{
	// Console::get().writeLine(JString(L"received warning ") + warningCode + " from client");
	FString str = "// received warning :: " + FString::FromInt(warningCode) + " from client";
	m_pView->ErrorCheckMessage(str, warningCode);
}

void SH_PhotonListener::serverErrorReturn(int errorCode)
{
	// Console::get().writeLine(JString(L"received error ") + errorCode + " from server");
	FString str = "// received error :: " + FString::FromInt(errorCode) + " from server";
	m_pView->ErrorCheckMessage(str, errorCode);
}