#pragma once

#ifdef __clang__
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif
#if defined(_EG_WINDOWS_PLATFORM)
#include "Windows/AllowWindowsPlatformTypes.h"
#endif
#pragma warning (disable: 4263)
#pragma warning (disable: 4264)
#include "LoadBalancing-cpp/inc/Client.h"
#pragma warning (default: 4263)
#pragma warning (default: 4264)
#if defined(_EG_WINDOWS_PLATFORM)
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#include "SH_PhotonBasic.h"
#include "CoreMinimal.h"

enum class EMoveEvent : uint8;
using namespace ExitGames;
using namespace Common;

class PhotonListner_Solaseado : public ExitGames::LoadBalancing::Listener
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

	// 커스텀 이벤트 
	virtual void customEventAction(int playerNr, nByte eventCode, const Common::Object& eventContentObj) override;

	// callbacks for operations on the server
	virtual void connectReturn(int errorCode, const Common::JString& errorString, const Common::JString& region, const Common::JString& cluster) override;
	virtual void disconnectReturn(void) override;

	// 방생성 이벤트 
	virtual void joinOrCreateRoomReturn(int localPlayerNr, const Common::Hashtable& roomProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString) override;
	virtual void createRoomReturn(int localPlayerNr, const Common::Hashtable& roomProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString) override;

	// 방 제거 or 떠나기
	virtual void leaveRoomReturn(int errorCode, const Common::JString& errorString) override;

	// 포톤 지역 설정 변경 
	virtual void onAvailableRegions(const JVector<JString>& availableRegions, const JVector<JString>& availableRegionServers) override;

	// 캐릭터 프로퍼티 업데이트 
	virtual void onPlayerPropertiesChange(int playerNr, const Common::Hashtable& changes) override;

	// 방 정보 프로퍼티 업데이트 
	virtual void onRoomPropertiesChange(const Common::Hashtable& changes) override;

	virtual void onRoomListUpdate(void) override;
protected:

	// 포톤 업데이트 시간 체크
	unsigned long lastUpdateTime;

	SH_PhotonBasic* m_pView;
	ExitGames::LoadBalancing::Client* m_pClient;
	// 접속 맵 
	ExitGames::Common::JString mMap;
	ExitGames::Common::JString mChannel = ExitGames::Common::JString("-1");

	// 플레이어 넘버
	int LocalPlayerNr;

	// 현재 그룹 정보
	int m_UseGroups;

public:

	void Connect(const JString& userName, const JString& serverAddress);

	// // Custom Event 
	// 위치 정보 
	void SetMoveAndRotation(int vX, int vY, int vZ, int yaw);		// 7

	void SetPlayerAnim(int Anim);									// 15

	// 텍스트 정보
	void PlayerTargetMessage(const int* target, int size, nByte text);	// 2
	void TextMessage(const JString& message, const JString& type);		// 1

	// 애니메이션D
	void setPlayerAnimationData(uint8 anim); // addCustomProperties

	// 방 진행중인 이벤트 체크
	void setRoomEventProperties(uint8 Ev);
	// 진행중인 이벤트 일시정지
	void setEventPause(bool ev);

	void SetFriendRequest(const int& Target, const FString& PlayFabID);

protected:

	//서버 수용 최대 인원수
	int MaxPlayerRoom = 15;

	//서버에 뿌릴 데이터를 저장해 둘 헤시 테이블
	//여기에 저장된 값들은 데이터가 뿌려지고 엑터 포톤의 updatePlayerProperties()에서 처리할 수 있게 한다.
	ExitGames::Common::Hashtable mCharacterInfo;

public:
	//플레이어 커스텀 데이터 정보 처리
	void SetChracterInfo(FString _key, FString _value);
	void SendCharacterInfo();
	void RemoveCharacterInfo();

	void InitJoinOrCreateRoom();
	void SendPlayerAnimState(uint8 _State);

//////////////////////룸정보, 변경
protected:
	FString sRoomName = "MainRoom";
	FString sRoomCount;
	bool b_IsFirstConnect = true;
	int RoomMaxSize = 20;
	int PlayerMaxCount = 20;

public:
	void ChangeRoomNumber(int Number) { sRoomCount = FString::FromInt(Number); }
	int getRoomCount() { return FCString::Atoi(*sRoomCount); }

///////////////////////더미여부 판단
protected:
	bool b_IsDummy = false;
public:
	void setDummy(bool IsDummy);

///////////////////////이동
public:
	void Move(FVector Loc);
	void MoveStop();
	void MoveStopFinish(FVector Loc);

/////NPC
	// NPC 업데이트
	void UpdateNPC(TArray<FString> aID, TArray<FVector> aLoc);
	void SetNPCMove(FString sName,FVector vLoc);
////NPC 끝
};


