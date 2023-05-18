#pragma once

#include"Photon\SH_PhotonListener.h"


class PhotonListner_Solaseado : public SH_PhotonListener
{
public:
	PhotonListner_Solaseado(SH_PhotonBasic* pView);
	virtual ~PhotonListner_Solaseado(void);
	virtual void service(void);

	void SetClient(ExitGames::LoadBalancing::Client* client);

	// receive and print out debug out here
	virtual void debugReturn(int debugLevel, const Common::JString& string) override;

	// implement your error-handling here
	virtual void connectionErrorReturn(int errorCode) override;
	virtual void clientErrorReturn(int errorCode) override;
	virtual void warningReturn(int warningCode) override;
	virtual void serverErrorReturn(int errorCode) override;

	// Room 
	// events, triggered by certain operations of all players in the same room
	virtual void joinRoomEventAction(int playerNr, const Common::JVector<int>& playernrs, const LoadBalancing::Player& player);
	virtual void leaveRoomEventAction(int playerNr, bool isInactive) override;

	// Ŀ���� �̺�Ʈ 
	virtual void customEventAction(int playerNr, nByte eventCode, const Common::Object& eventContentObj) override;

	// callbacks for operations on the server
	virtual void connectReturn(int errorCode, const Common::JString& errorString, const Common::JString& region, const Common::JString& cluster) override;
	virtual void disconnectReturn(void) override;

	// ����� �̺�Ʈ 
	virtual void joinOrCreateRoomReturn(int localPlayerNr, const Common::Hashtable& roomProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString) override;
	virtual void createRoomReturn(int localPlayerNr, const Common::Hashtable& roomProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString) override;

	// �� ���� or ������
	virtual void leaveRoomReturn(int errorCode, const Common::JString& errorString) override;

	// ���� ���� ���� ���� 
	virtual void onAvailableRegions(const JVector<JString>& availableRegions, const JVector<JString>& availableRegionServers) override;

	// ĳ���� ������Ƽ ������Ʈ 
	virtual void onPlayerPropertiesChange(int playerNr, const Common::Hashtable& changes) override;

	// �� ���� ������Ƽ ������Ʈ 
	virtual void onRoomPropertiesChange(const Common::Hashtable& changes) override;
protected:

	// ���� ������Ʈ �ð� üũ
	unsigned long lastUpdateTime;

	SH_PhotonBasic* m_pView;
	ExitGames::LoadBalancing::Client* m_pClient;
	// ���� �� 
	ExitGames::Common::JString mMap;
	ExitGames::Common::JString mChannel = ExitGames::Common::JString("-1");

	// �÷��̾� �ѹ�
	int LocalPlayerNr;

	// ���� �׷� ����
	int m_UseGroups;

public:

	void Connect(const JString& userName, const JString& serverAddress);

	// // Custom Event 
	// ��ġ ���� 
	void SetMovePlayer(int vX, int vY, int vz);	// 6
	void SetMovePlayerRotation(float fZ);		// 7

	// �ؽ�Ʈ ����
	void PlayerTargetMessage(const int* target, int size, nByte text);	// 2
	void TextMessage(const JString& message, const JString& type);		// 1

	// �ִϸ��̼�
	void setPlayerAnimationData(uint8 anim); // addCustomProperties

	// �� �������� �̺�Ʈ üũ
	void setRoomEventProperties(uint8 Ev);
	// �������� �̺�Ʈ �Ͻ�����
	void setEventPause(bool ev);

protected:
	//���� ���� �ִ� �ο���
	int MaxPlayerRoom = 15;
	//������ �Ѹ� �����͸� ������ �� ��� ���̺�
	//���⿡ ����� ������ �����Ͱ� �ѷ����� ���� ������ updatePlayerProperties()���� ó���� �� �ְ� �Ѵ�.
	ExitGames::Common::Hashtable mCharacterInfo;
	ExitGames::Common::JString sRoomName="InitRoomName";
public:
	//�÷��̾� Ŀ���� ������ ���� ó��
	void SetChracterInfo(FString _key, FString _value);
	void SendCharacterInfo();
	void RemoveCharacterInfo();

	void InitJoinOrCreateRoom();

	void SendPlayerAnimState(/*uint8 _State*/);
};


