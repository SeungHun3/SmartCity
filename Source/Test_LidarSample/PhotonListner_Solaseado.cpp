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


// ���� ���� ���� �ο����� ���� �̺�Ʈ�� �߻��Ѵٰ� �˸���
void PhotonListner_Solaseado::joinRoomEventAction(int playerNr, const Common::JVector<int>& playernrs, const LoadBalancing::Player& player)
{
	// �� ���ӽ� ��Ƽ���� ��ε��ɽ��� �Լ�
	bool local = player.getNumber() == m_pClient->getLocalPlayer().getNumber();
	if (local)
		return;

	// ���� ����
	Hashtable table = player.getCustomProperties();
	//UE_LOG(LogTemp, Log, TEXT("// joinRoomEventAction :: %d"), playerNr);
	m_pView->AddPlayers(playerNr, player.getName().UTF8Representation().cstr(), local, table);
	//������ �ο����� ĳ���� ������ ����
	m_pView->updateLocalPlayerPosion();
}


// �� ������ // ���� �������� �� �÷��̾� ���� �����ٰ� �˸���
void PhotonListner_Solaseado::leaveRoomEventAction(int playerNr, bool isInactive)
{
	// UE_LOG(LogTemp, Log, TEXT("// Leave Player Nr :: %d"), playerNr);
	m_pView->RemovePlayer(playerNr);
}


// ������ ������ ���� 
void PhotonListner_Solaseado::onPlayerPropertiesChange(int playerNr, const Common::Hashtable& changes)
{
	//UE_LOG(LogTemp, Warning, TEXT("// onPlayerPropertiesChange :: (%d)"), playerNr);
	m_pView->updatePlayerProperties(playerNr, changes);
}

// (LiDAR) �� ���� ���� // ev , notice
void PhotonListner_Solaseado::onRoomPropertiesChange(const Common::Hashtable& changes)
{
	m_pView->updateRoomProperties(changes);
}


// ���� ����
void PhotonListner_Solaseado::disconnectReturn(void)
{
	m_pView->InitPlayers();
	m_pView->ErrorCheckMessage("// DisconnectReturn :: ", 99);
}

// ���� ���� 
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

		// �������� �� �⺻ ����
		JString Name = myRoom.getName();
		nByte Count = myRoom.getPlayerCount();
		nByte Maxcount = myRoom.getMaxPlayers();
		m_pView->CurrentRoomInfo(Name, Count, Maxcount);

		
		//FString str = UTF8_TO_TCHAR(p->getName().UTF8Representation().cstr());
		//UE_LOG(LogTemp, Log, TEXT("// AddPlayer count :: %d, Max Count : %d"), Count, Maxcount);


		// FString str = "Ev";
		// ���̴� �̺�Ʈ ��� // ������ ���̺� quiz Row number
		m_pView->updateRoomProperties(props);

		// ���� �濡 �ִ� �÷��̾� ���� ��� 
		const JVector<Player*>& players = myRoom.getPlayers();
		int size = players.getSize();
		for (int i = 0; i < size; ++i)
		{
			const Player* p = players[i];
			bool local = p->getNumber() == m_pClient->getLocalPlayer().getNumber();

			// ���� ����
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
	//�ϴ� �⺻�� �̰Ű� �ƴ� joinorcreatee������


	if (errorCode == ErrorCode::OK)
	{
		int playersize = playerProperties.getSize();
		//UE_LOG(LogTemp, Log, TEXT("// joinOrCreateRoomReturn :: playerProperties.getSize() :: %d"), playersize);

		// this->LocalPlayerNr = localPlayerNr;
		MutableRoom& myRoom = m_pClient->getCurrentlyJoinedRoom();
		Hashtable props = myRoom.getCustomProperties();

		// �������� �� �⺻ ����
		JString Name = myRoom.getName();
		nByte Count = myRoom.getPlayerCount();
		nByte Maxcount = myRoom.getMaxPlayers();
		m_pView->CurrentRoomInfo(Name, Count, Maxcount);

		// FString str = "Ev";
		// ���̴� �̺�Ʈ ��� // ������ ���̺� quiz Row number
		m_pView->updateRoomProperties(props);

		// ���� �濡 �ִ� �÷��̾� ���� ��� 
		const JVector<Player*>& players = myRoom.getPlayers();
		int size = players.getSize();
		for (int i = 0; i < size; ++i)
		{
			const Player* p = players[i];
			bool local = p->getNumber() == m_pClient->getLocalPlayer().getNumber();

			// ���� ����
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
	else if (eventCode == 6) // vector3 ��ġ ������
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
		if (obj && obj->getDimensions() == 1) // float ĳ���� ȸ�� ������
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
		if (obj && obj->getDimensions() == 1) // �̺�Ʈ �Ͻ�����
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
		//�Է¹��� Ŀ�ǵ�� ������ ��ȯ ��ǥ��
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
				//�ӽ÷� vX,vY�� ǥ���Ѵ�.
				return;
			}
		}
	}
}

// �� ������ ����
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


// ��ġ ���� // Location & Rotation 
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

//�̵� ����ȭ�� ���� ���� Ŀ�ǵ� �Է°� ������ ���� ���� ��ġ���� �Է��մϴ�.
void PhotonListner_Solaseado::SetPlayerMoveCommand(float vX, float vY, float lerpX, float lerpY)
{
	Hashtable data;
	float coords[] = { static_cast<float>(vX) , static_cast<float>(vY),static_cast<float>(lerpX) , static_cast<float>(lerpY) };
	data.put((nByte)1, coords, 4);
	m_pClient->opRaiseEvent(false, data, 15);
}

// Ư�� �������� ������ �޼��� 
void PhotonListner_Solaseado::PlayerTargetMessage(const int* target, int size, nByte text)
{
	Hashtable data;
	nByte coords[] = { static_cast<nByte>(text) };
	data.put((nByte)1, coords, 1);
	RaiseEventOptions option;
	option.setTargetPlayers(target, size);
	m_pClient->opRaiseEvent(false, data, 2, option);
}

// ���� �޽��� 
void PhotonListner_Solaseado::TextMessage(const JString& message, const JString& type)
{
	Hashtable data;
	JString coords[] = { static_cast<JString>(message)  , static_cast<JString>(type) };
	data.put((nByte)1, coords, 2);
	RaiseEventOptions Options;
	Options.setReceiverGroup(ExitGames::Lite::ReceiverGroup::ALL);
	m_pClient->opRaiseEvent(false, data, 1, Options);
}

// ĳ���� ������, �ִϸ��̼�
void PhotonListner_Solaseado::setPlayerAnimationData(uint8 anim)
{
	MutablePlayer& player = m_pClient->getLocalPlayer();
	Hashtable table;

	FString str = "An";
	table.put(TCHAR_TO_UTF8(*str), anim);

	// UE_LOG(LogTemp, Log, TEXT("// Change Animation :: %d"), anim);
	player.addCustomProperties(table);
}


// ( LiDAR ) RoomProperties // �̺�Ʈ, �����޼��� 
// ( LiDAR ) �̺�Ʈ ���� ���  // "Ev" >> �̺�Ʈ ������ ���̺� (Data_LiDARQuiz) ��ѹ�
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



// ���� ���� ó�� 
void PhotonListner_Solaseado::connectionErrorReturn(int errorCode)
{
	m_pView->ErrorCheckMessage("// Connection failed with error", errorCode);
}


/**	���� ���� ���� ó��
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


//�Է¹��� ĳ���� ������ mCharacterInfo�� �����Ѵ�.
void PhotonListner_Solaseado::SetChracterInfo(FString _key, FString _value)
{
	//UE_LOG(LogTemp, Log, TEXT("//SetChracterInfo ( Key : %s, Value %s )"),*_key,*_value);
	mCharacterInfo.put(TCHAR_TO_UTF8(*_key), TCHAR_TO_UTF8(*_value));
}


//�����ص� ĳ���� �����͸� �÷��̾�鿡�� �ѷ��ش�.
void PhotonListner_Solaseado::SendCharacterInfo()
{
	//UE_LOG(LogTemp, Log, TEXT("//SendCharacterInfo()"));
	m_pClient->getLocalPlayer().addCustomProperties(mCharacterInfo);
	//�����͸� �������� ���� ä���α� ���ؼ� ����.
	RemoveCharacterInfo();
}


//�����ص� ĳ���� �����͸� ����ش�.
void PhotonListner_Solaseado::RemoveCharacterInfo()
{
	//UE_LOG(LogTemp, Log, TEXT("//RemoveCharacterInfo()"));
	mCharacterInfo.removeAllElements();
}


//ó�� �����ϰ� �����͸� ������ �濡 �����մϴ�.
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


//�ִϸ��̼� ���� �����͸� �����ִ� �Լ�
void PhotonListner_Solaseado::SendPlayerAnimState(uint8 _State)
{
	FString Estr = "An";
	mCharacterInfo.put(TCHAR_TO_UTF8(*Estr), _State);
	m_pClient->getLocalPlayer().addCustomProperties(mCharacterInfo);
	////�����͸� �������� ���� ä���α� ���ؼ� ����.
	RemoveCharacterInfo();
}


