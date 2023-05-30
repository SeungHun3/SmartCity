#include"PhotonListner_Solaseado.h"

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

#include "CoreMinimal.h"

using namespace ExitGames::Common;
using namespace ExitGames::LoadBalancing;

PhotonListner_Solaseado::PhotonListner_Solaseado(SH_PhotonBasic* pView) :SH_PhotonListener(pView)
{
	m_pView = pView;
	m_pClient = NULL;
	LocalPlayerNr = 0;
}

PhotonListner_Solaseado::~PhotonListner_Solaseado(void)
{

}


void PhotonListner_Solaseado::SetClient(ExitGames::LoadBalancing::Client* client)
{
	this->m_pClient = client;
}

void PhotonListner_Solaseado::service(void)
{
	if (m_pClient)
		m_pClient->service();
}

void PhotonListner_Solaseado::Connect(const JString& userName, const JString& serverAddress)
{
	m_pClient->setAutoJoinLobby(true);
	m_pClient->connect(ConnectOptions(AuthenticationValues().setUserID(JString() + GETTIMEMS()), userName, serverAddress));
	
}

void PhotonListner_Solaseado::onAvailableRegions(const JVector<JString>& availableRegions, const JVector<JString>& availableRegionServers)
{

	int size = availableRegions.getSize();
	for (int i = 0; i < size; i++)
	{
		FString str = availableRegions[i].UTF8Representation().cstr();
	}
	m_pView->setRegion();
}


// 현재 접속 중인 인원에게 접속 이벤트가 발생한다고 알린다
void PhotonListner_Solaseado::joinRoomEventAction(int playerNr, const Common::JVector<int>& playernrs, const LoadBalancing::Player& player)
{
	// 방 접속시 멀티유저 브로드케스팅 함수
	bool local = player.getNumber() == m_pClient->getLocalPlayer().getNumber();
	if (local)
		return;

	// 외형 정보
	Hashtable table = player.getCustomProperties();
	//UE_LOG(LogTemp, Log, TEXT("// joinRoomEventAction :: %d"), playerNr);
	m_pView->AddPlayers(playerNr, player.getName().UTF8Representation().cstr(), local, table);
	//접속한 인원에게 캐릭터 정보값 갱신
	m_pView->updateLocalPlayerPosion();
}


// 방 떠나기 // 현재 접속중인 방 플레이어 에게 떠난다고 알리기
void PhotonListner_Solaseado::leaveRoomEventAction(int playerNr, bool isInactive)
{
	// UE_LOG(LogTemp, Log, TEXT("// Leave Player Nr :: %d"), playerNr);
	m_pView->RemovePlayer(playerNr);
}


// 접속자 데이터 변경 
void PhotonListner_Solaseado::onPlayerPropertiesChange(int playerNr, const Common::Hashtable& changes)
{
	//UE_LOG(LogTemp, Warning, TEXT("// onPlayerPropertiesChange :: (%d)"), playerNr);
	m_pView->updatePlayerProperties(playerNr, changes);
}

// (LiDAR) 방 정보 변경 // ev , notice
void PhotonListner_Solaseado::onRoomPropertiesChange(const Common::Hashtable& changes)
{
	m_pView->updateRoomProperties(changes);
}


// 연결 종료
void PhotonListner_Solaseado::disconnectReturn(void)
{
	m_pView->InitPlayers();
	m_pView->ErrorCheckMessage("// DisconnectReturn :: ", 99);
}

// 접속 성공 
void PhotonListner_Solaseado::connectReturn(int errorCode, const Common::JString& errorString, const Common::JString& region, const Common::JString& cluster)
{
	if (errorCode == ErrorCode::OK)
	{
		FString str = m_pClient->getLocalPlayer().getName().UTF8Representation().cstr();

		m_pView->ConnectComplete();
	}
}
void PhotonListner_Solaseado::joinOrCreateRoomReturn(int localPlayerNr, const Common::Hashtable& roomProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	if (errorCode == ErrorCode::OK)
	{
		int playersize = playerProperties.getSize();
		//UE_LOG(LogTemp, Log, TEXT("// joinOrCreateRoomReturn :: playerProperties.getSize() :: %d"), playersize);

		// this->LocalPlayerNr = localPlayerNr;
		MutableRoom& myRoom = m_pClient->getCurrentlyJoinedRoom();
		Hashtable props = myRoom.getCustomProperties();

		// 접속중인 방 기본 정보
		JString Name = myRoom.getName();
		nByte Count = myRoom.getPlayerCount();
		nByte Maxcount = myRoom.getMaxPlayers();
		m_pView->CurrentRoomInfo(Name, Count, Maxcount);

		
		//FString str = UTF8_TO_TCHAR(p->getName().UTF8Representation().cstr());
		//UE_LOG(LogTemp, Log, TEXT("// AddPlayer count :: %d, Max Count : %d"), Count, Maxcount);


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
		m_pView->JoinOrCreateComplete();
	}
	else
	{
		FString str = UTF8_TO_TCHAR(errorString.UTF8Representation().cstr());
		//UE_LOG(LogTemp, Log, TEXT("// joinOrCreateRoomReturn Error :: %s , %d"), *str, localPlayerNr);
	}
}

void PhotonListner_Solaseado::createRoomReturn(int localPlayerNr, const Common::Hashtable& roomProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	//일단 기본은 이거가 아닌 joinorcreatee룸으로


	if (errorCode == ErrorCode::OK)
	{
		int playersize = playerProperties.getSize();
		//UE_LOG(LogTemp, Log, TEXT("// joinOrCreateRoomReturn :: playerProperties.getSize() :: %d"), playersize);

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
		m_pView->JoinOrCreateComplete();
	}
	else
	{
		FString str = UTF8_TO_TCHAR(errorString.UTF8Representation().cstr());
		//UE_LOG(LogTemp, Log, TEXT("// joinOrCreateRoomReturn Error :: %s , %d"), *str, errorCode);
	}
}

void PhotonListner_Solaseado::customEventAction(int playerNr, nByte eventCode, const Common::Object& eventContentObj)
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
		//UE_LOG(LogTemp, Log, TEXT("// %d eventCode == 6 "), playerNr);
		if (obj && obj->getDimensions() == 1)
		{
			int* data = ((ValueObject<int*>*)obj)->getDataCopy();
			
			int vX = data[0];
			int vY = data[1];
			int vZ = data[2];

			//UE_LOG(LogTemp, Log, TEXT("// %d"), sizeof(*data));

			UE_LOG(LogTemp, Log, TEXT("// eventCode == 6 :: x: %d,y: %d,z: %d"), vX, vY, vZ);

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
	else if (eventCode == 15)
	{
		//입력받은 커맨드와 보간을 위환 좌표값
		if (obj && obj->getDimensions() == 1) 
		{
			if (obj->getType() == TypeCode::EG_FLOAT)
			{
				float* data = ((ValueObject<float*>*)obj)->getDataCopy();

				float vX = data[0];
				float vY = data[1];
				float lerpX = data[2];
				float lerpY = data[3];

				m_pView->GetMovePlayerXYandLeryXY(playerNr,vX,vY, lerpX, lerpY);
				UE_LOG(LogTemp, Log, TEXT("// eventCode == 15 (%f, %f) lerp: (%f,%f)"), vX, vY, lerpX, lerpY);
				//임시로 vX,vY로 표기한다.
				return;
			}
		}
	}
}

// 방 떠나기 성공
void PhotonListner_Solaseado::leaveRoomReturn(int errorCode, const Common::JString& errorString)
{
	if (errorCode == ErrorCode::OK)
	{
		m_pView->LeaveRoomComplete();
		m_pView->InitPlayers();
	}
	else
	{
		FString str = UTF8_TO_TCHAR(errorString.UTF8Representation().cstr());
		//UE_LOG(LogTemp, Warning, TEXT("// LeaveRoom Return Error :: %s , %d"), *str, errorCode);
	}
}


// 위치 전송 // Location & Rotation 
void PhotonListner_Solaseado::SetMovePlayer(int vX, int vY, int vz)
{
	UE_LOG(LogTemp, Log, TEXT("// SetMovePlayer :: x: %d,y: %d,z: %d"), vX, vY, vz);

	Hashtable HashData;
	int coords[] = { static_cast<int>(vX) , static_cast<int>(vY) ,static_cast<int>(vz) };
	HashData.put((nByte)1, coords, 3);
	m_pClient->opRaiseEvent(false, HashData, 6);
}

// Rotation 
void PhotonListner_Solaseado::SetMovePlayerRotation(float fZ)
{
	Hashtable data;
	float coords[] = { static_cast<float>(fZ) };
	data.put((nByte)1, coords, 1);
	m_pClient->opRaiseEvent(false, data, 7);
}

//이동 동기화를 위한 방향 커맨드 입력과 보간을 위한 현재 위치값을 입력합니다.
void PhotonListner_Solaseado::SetPlayerMoveCommand(float vX, float vY, float lerpX, float lerpY)
{
	Hashtable data;
	float coords[] = { static_cast<float>(vX) , static_cast<float>(vY),static_cast<float>(lerpX) , static_cast<float>(lerpY) };
	data.put((nByte)1, coords, 4);
	m_pClient->opRaiseEvent(false, data, 15);
}

// 특정 유저에게 보내는 메세지 
void PhotonListner_Solaseado::PlayerTargetMessage(const int* target, int size, nByte text)
{
	Hashtable data;
	nByte coords[] = { static_cast<nByte>(text) };
	data.put((nByte)1, coords, 1);
	RaiseEventOptions option;
	option.setTargetPlayers(target, size);
	m_pClient->opRaiseEvent(false, data, 2, option);
}

// 월드 메시지 
void PhotonListner_Solaseado::TextMessage(const JString& message, const JString& type)
{
	Hashtable data;
	JString coords[] = { static_cast<JString>(message)  , static_cast<JString>(type) };
	data.put((nByte)1, coords, 2);
	RaiseEventOptions Options;
	Options.setReceiverGroup(ExitGames::Lite::ReceiverGroup::ALL);
	m_pClient->opRaiseEvent(false, data, 1, Options);
}

// 캐릭터 움직임, 애니메이션
void PhotonListner_Solaseado::setPlayerAnimationData(uint8 anim)
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
void PhotonListner_Solaseado::setRoomEventProperties(uint8 Ev)
{
	MutableRoom& CheckRoom = m_pClient->getCurrentlyJoinedRoom();
	Hashtable table;

	FString str = "Ev";
	table.put(TCHAR_TO_UTF8(*str), Ev);

	// UE_LOG(LogTemp, Log, TEXT("// Add Room Custom Properties :: %d"), Ev);
	CheckRoom.addCustomProperties(table);
}

void PhotonListner_Solaseado::setEventPause(bool ev)
{
	Hashtable data;
	bool coords[] = { static_cast<bool>(ev) };
	data.put((nByte)1, coords, 1);
	RaiseEventOptions Options;
	Options.setReceiverGroup(ExitGames::Lite::ReceiverGroup::ALL);
	m_pClient->opRaiseEvent(false, data, 11, Options);
}



// 접속 오류 처리 
void PhotonListner_Solaseado::connectionErrorReturn(int errorCode)
{
	m_pView->ErrorCheckMessage("// Connection failed with error", errorCode);
}


/**	서버 에러 문구 처리
	*/
void PhotonListner_Solaseado::debugReturn(int debugLevel, const Common::JString& string)
{
	// Console::get().debugReturn(debugLevel, string);
	FString str = UTF8_TO_TCHAR(string.UTF8Representation().cstr());
	m_pView->ErrorCheckMessage(str, debugLevel);

	
}


void PhotonListner_Solaseado::clientErrorReturn(int errorCode)
{
	// Console::get().writeLine(JString(L"received error ") + errorCode + L" from client");
	FString str = "// received error :: " + FString::FromInt(errorCode) + " from client";
	m_pView->ErrorCheckMessage(str, errorCode);
}

void PhotonListner_Solaseado::warningReturn(int warningCode)
{
	// Console::get().writeLine(JString(L"received warning ") + warningCode + " from client");
	FString str = "// received warning :: " + FString::FromInt(warningCode) + " from client";
	m_pView->ErrorCheckMessage(str, warningCode);
}

void PhotonListner_Solaseado::serverErrorReturn(int errorCode)
{
	// Console::get().writeLine(JString(L"received error ") + errorCode + " from server");
	FString str = "// received error :: " + FString::FromInt(errorCode) + " from server";
	m_pView->ErrorCheckMessage(str, errorCode);
}


//입력받은 캐릭터 정보를 mCharacterInfo에 저장한다.
void PhotonListner_Solaseado::SetChracterInfo(FString _key, FString _value)
{
	//UE_LOG(LogTemp, Log, TEXT("//SetChracterInfo ( Key : %s, Value %s )"),*_key,*_value);
	mCharacterInfo.put(TCHAR_TO_UTF8(*_key), TCHAR_TO_UTF8(*_value));
}


//저장해둔 캐릭터 데이터를 플레이어들에게 뿌려준다.
void PhotonListner_Solaseado::SendCharacterInfo()
{
	//UE_LOG(LogTemp, Log, TEXT("//SendCharacterInfo()"));
	m_pClient->getLocalPlayer().addCustomProperties(mCharacterInfo);
	//데이터를 보냈으니 새로 채워두기 위해서 비운다.
	RemoveCharacterInfo();
}


//저장해둔 캐릭터 데이터를 비워준다.
void PhotonListner_Solaseado::RemoveCharacterInfo()
{
	//UE_LOG(LogTemp, Log, TEXT("//RemoveCharacterInfo()"));
	mCharacterInfo.removeAllElements();
}


//처음 접속하고 데이터를 보내고 방에 접속합니다.
void PhotonListner_Solaseado::InitJoinOrCreateRoom()
{
	//UE_LOG(LogTemp, Log, TEXT("//CustomJoinOrCreateRoom()"));
	m_pClient->getLocalPlayer().addCustomProperties(mCharacterInfo);
	RemoveCharacterInfo();

	RoomOptions options;
	options.setMaxPlayers(MaxPlayerRoom);
	options.setPublishUserID(true);
	m_pClient->opJoinOrCreateRoom(sRoomName, options);
}


//애니메이션 상태 데이터를 보내주는 함수
void PhotonListner_Solaseado::SendPlayerAnimState(uint8 _State)
{
	FString Estr = "An";
	mCharacterInfo.put(TCHAR_TO_UTF8(*Estr), _State);
	m_pClient->getLocalPlayer().addCustomProperties(mCharacterInfo);
	////데이터를 보냈으니 새로 채워두기 위해서 비운다.
	RemoveCharacterInfo();
}


