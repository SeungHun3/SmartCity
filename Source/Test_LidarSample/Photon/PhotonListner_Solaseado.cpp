
#include "PhotonListner_Solaseado.h"

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

PhotonListner_Solaseado::PhotonListner_Solaseado(SH_PhotonBasic* pView)
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

	m_pView->setRegion(m_pClient);
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
	m_pView->AddPlayers(playerNr, player.getName().UTF8Representation().cstr(), local, table);

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

	//UE_LOG(LogTemp, Warning, TEXT("// onRoomPropertiesChange :: (%d)"), m_pClient->getLocalPlayer().getNumber());
}

//채널 뷰 설정 세팅 // channel = 1 ~ 20 // 총 20개의 룸 탐색함수 // RoomMaxSize; 
void PhotonListner_Solaseado::onRoomListUpdate(void) // Room프로퍼티가 변경될때마다 호출 // ex)방생성, 삭제, 인원수변동
{
	TMap<int, int> RoomMap; // channel, PlayerCount;

	// dummy일 경우에만
	if (b_IsDummy) 
	{
		int RoomListSize = m_pClient->getRoomList().getSize(); // 룸배열은 먼저 생성된 순서대로 설정됨
		for (int i = 0; i < RoomListSize; i++)
		{
			FString Channel;
			FString RoomName = m_pClient->getRoomList()[i]->getName().UTF8Representation().cstr();
			int Index = RoomName.Len() - sRoomName.Len();
			while (Index--)
			{
				Channel += RoomName[(RoomName.Len() - 1) - Index];				//채널		
			}
			int PlayerCount = m_pClient->getRoomList()[i]->getPlayerCount();	//플레이어수 
			RoomMap.Add(FCString::Atoi(*Channel), PlayerCount);					// 맵에 넣어서
		}
		m_pView->UpdateRoomList(RoomMap);										// 액터로 전송

		return;
	}
	// 더미가 아닌 메인클라이언트가 첫접속시에
	if(b_IsFirstConnect) 
	{
		b_IsFirstConnect = false;
		TArray<int> ChannelArray;
		TArray<int> PlayerCountArray;

		int RoomSize = m_pClient->getRoomList().getSize(); // 룸배열은 먼저 생성된 순서대로 설정됨
		for (int i = 0; i < RoomSize; i++)
		{
			FString Channel;
			FString RoomName = m_pClient->getRoomList()[i]->getName().UTF8Representation().cstr();
			int Index = RoomName.Len() - sRoomName.Len();
			while (Index--)
			{
				Channel += RoomName[(RoomName.Len() - 1) - Index];				//채널		
			}
			int PlayerCount = m_pClient->getRoomList()[i]->getPlayerCount();	//플레이어수 
			RoomMap.Add(FCString::Atoi(*Channel), PlayerCount);					// 맵에 넣어서
		}
	
		for (int i = 1; i <= RoomMaxSize; i++)									// 룸탐색
		{
			if (RoomMap.Contains(i)) // 룸을 찾았다면
			{
				//최대 인원수 체크
				int PlayerCount = *RoomMap.Find(i);
				if (PlayerCount <= PlayerMaxCount)
				{
					sRoomCount = FString::FromInt(i);
					m_pView->ConnectComplete(); // 방 입장
					return;
				}
			}
			else//찾지 못했다면 룸이 없음 ->  해당번호 룸입장
			{
				sRoomCount = FString::FromInt(i);
				m_pView->ConnectComplete(); // 방 입장
				return;
			}
		}
		// 전부다 풀방이라면

		sRoomCount = "Full";
		m_pView->ConnectComplete();
		return;
	}
}

// 연결 종료
void PhotonListner_Solaseado::disconnectReturn(void)
{
	if (b_IsDummy)
	{
		return;
	}
	m_pView->InitPlayers();
	m_pView->ErrorCheckMessage("// DisconnectReturn :: ", 99);
}

// 접속 성공 (로직변경) // connectReturn이 RoomListUpdate보다 빨리 호출해서 방정보 검색 불가능 -> RoomListUpdate로 방검색 후 접속
void PhotonListner_Solaseado::connectReturn(int errorCode, const Common::JString& errorString, const Common::JString& region, const Common::JString& cluster)
{
	//onRoomListUpdate -> connectComplete(Channel)
}


void PhotonListner_Solaseado::joinOrCreateRoomReturn(int localPlayerNr, const Common::Hashtable& roomProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	if (errorCode == ErrorCode::OK && !b_IsDummy)
	{
		int playersize = playerProperties.getSize();
		//UE_LOG(LogTemp, Log, TEXT("// joinOrCreateRoomReturn :: playerProperties.getSize() :: %d"), playersize);

		//this->LocalPlayerNr = localPlayerNr;
		MutableRoom& myRoom = m_pClient->getCurrentlyJoinedRoom();
		Hashtable props = myRoom.getCustomProperties();

		// 접속중인 방 기본 정보
		JString CurRoomName = myRoom.getName();
		nByte Count = myRoom.getPlayerCount();
		nByte Maxcount = myRoom.getMaxPlayers();
		m_pView->CurrentRoomInfo(CurRoomName, Count, Maxcount);

		//이전 채널의 NPC가 남아있다면 제거한다.
		m_pView->ClearNpc();

		m_pView->updateRoomProperties(props);

		//첫 접속에 마스터 플레이어일 경우엔 방을 생성하는 경우이므로 이때 NPC들을 스폰한다.
		if (myRoom.getMasterClientID() == localPlayerNr)
		{
			//데이터 테이블에 있는 NPC들을 소환한다.
			m_pView->InitNpc();
		}

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

		FString FRoomName = CurRoomName.UTF8Representation().cstr();
		m_pView->JoinOrCreateComplete(FRoomName);
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
		FString RoomNumber = Name.UTF8Representation().cstr();
		m_pView->JoinOrCreateComplete(RoomNumber);
	}
	else
	{
		FString str = UTF8_TO_TCHAR(errorString.UTF8Representation().cstr());
		//UE_LOG(LogTemp, Log, TEXT("// joinOrCreateRoomReturn Error :: %s , %d"), *str, errorCode);
	}
}

void PhotonListner_Solaseado::customEventAction(int playerNr, nByte eventCode, const Common::Object& eventContentObj)
{
	if (b_IsDummy)
	{
		return;
	}

	ExitGames::Common::Hashtable eventContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();
	
	Object const* obj = eventContent.getValue("1");
	if (!obj)	obj = eventContent.getValue((nByte)1);
	if (!obj)	obj = eventContent.getValue(1);
	if (!obj)	obj = eventContent.getValue(1.0);

	if (!obj)
	{
		return;
	}

	else if (eventCode == 8)
	{
		if (obj->getDimensions() == 1)
		{
			int* data = ((ValueObject<int*>*)obj)->getDataCopy();

			int vX = data[0];
			int vY = data[1];
			int vZ = data[2];
			int yaw = data[3];

			m_pView->GetMoveAndRotation(playerNr,vX,vY,vZ,yaw);

			return;
		}
	}
	else if (eventCode == 11)
	{
		if (obj->getDimensions() == 1) // 이벤트 일시정지
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
	//친구 신청
	else if (eventCode == 12)
	{
		if (obj->getDimensions() == 1) // 친구 신청
		{
			JString* data = ((ValueObject<JString*>*)obj)->getDataCopy();
			FString PlayFabID = FString(UTF8_TO_TCHAR(data[0].UTF8Representation().cstr()));

			m_pView->RecvFriendRequest(playerNr,PlayFabID);
			return;
			
		}
	}
	//친구 수락
	else if (eventCode == 13)
	{
		if (obj->getDimensions() == 1) // 이벤트 일시정지
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
	//애님값
	else if (eventCode == 15)
	{
		if (obj->getDimensions() == 1)
		{
			if (obj->getType() == TypeCode::INTEGER)
			{
				int* data = ((ValueObject<int*>*)obj)->getDataCopy();

				int vX = data[0];

				m_pView->GetPlayerAnim(playerNr, vX);
				return;
			}
		}
	}
	//MoveForward
	else if (eventCode == 50)
	{
		//UE_LOG(LogTemp, Log, TEXT("// %d eventCode == 6 "), playerNr);
		if (obj->getDimensions() == 1)
		{
			float* data = ((ValueObject<float*>*)obj)->getDataCopy();
			FVector PlayerLoc;
			PlayerLoc.X = data[0];
			PlayerLoc.Y = data[1];
			PlayerLoc.Z = data[2];
			
			m_pView->UpdateMove(playerNr, PlayerLoc);
			return;
		}
	}
	//MoveStop
	else if (eventCode == 51)
	{
		m_pView->UpdateStop(playerNr);
	}
	//StopFinesh
	else if (eventCode == 52)
	{
		if (obj->getDimensions() == 1)
		{
			float* data = ((ValueObject<float*>*)obj)->getDataCopy();
			FVector PlayerLoc;
			PlayerLoc.X = data[0];
			PlayerLoc.Y = data[1];
			PlayerLoc.Z = data[2];
			m_pView->UpdateStopFinished(playerNr, PlayerLoc);
			return;
		}
	}
}

// 방 떠나기 성공
void PhotonListner_Solaseado::leaveRoomReturn(int errorCode, const Common::JString& errorString)
{
	if (errorCode == ErrorCode::OK && !b_IsDummy)
	{
		m_pView->InitPlayers();
		m_pView->LeaveRoomComplete();
	}
}

void PhotonListner_Solaseado::SetMoveAndRotation(int vX, int vY, int vZ, int yaw)
{
	Hashtable data;
	int coords[] = { static_cast<int>(vX) ,static_cast<int>(vY) ,static_cast<int>(vZ) ,static_cast<int>(yaw) };
	data.put((nByte)1, coords, 4);

	m_pClient->opRaiseEvent(false, data, 8);
}

void PhotonListner_Solaseado::SetPlayerAnim(int Anim)
{
	Hashtable data;
	int coords[] = { static_cast<int>(Anim) };
	data.put((nByte)1, coords, 1);
	RaiseEventOptions option;
	option.setReceiverGroup(ExitGames::Lite::ReceiverGroup::ALL);
	//option.
	m_pClient->opRaiseEvent(false, data, 15, option);
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

void PhotonListner_Solaseado::SetFriendRequest(const int& Target ,const FString& PlayFabID)
{
	Hashtable data;
	JString coords[] = { static_cast<JString>(TCHAR_TO_UTF8(*PlayFabID)) };
	data.put((nByte)1, coords, 1);
	//
	RaiseEventOptions Options;
	Options.setTargetPlayers(&Target, 1);
	m_pClient->opRaiseEvent(false, data, 12, Options);
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
	// 룸이름 세팅
	FString FmyRoom = sRoomName + sRoomCount;
	ExitGames::Common::JString JmyRoom = TCHAR_TO_UTF8(*FmyRoom);
	
	m_pClient->getLocalPlayer().addCustomProperties(mCharacterInfo);
	RemoveCharacterInfo();
	
	RoomOptions options;
	options.setMaxPlayers(MaxPlayerRoom);
	options.setPublishUserID(true);
	m_pClient->opJoinOrCreateRoom(JmyRoom, options);
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

//Dummy 세팅
void PhotonListner_Solaseado::setDummy(bool IsDummy)
{
	b_IsDummy = IsDummy;
}

void PhotonListner_Solaseado::Move(FVector Loc)
{
	float MoveX = Loc.X;
	float MoveY = Loc.Y;
	float MoveZ = Loc.Z;
	Hashtable data;

	float coords[] = { static_cast<float>(MoveX),static_cast<float>(MoveY),static_cast<float>(MoveZ)};
	data.put((nByte)1, coords, 3);
	RaiseEventOptions option;
	option.setReceiverGroup(ExitGames::Lite::ReceiverGroup::ALL);
	
	m_pClient->opRaiseEvent(false, data, 50, option);
}

void PhotonListner_Solaseado::MoveStop()
{
	Hashtable data;
	data.put((nByte)1);
	RaiseEventOptions option;
	option.setReceiverGroup(ExitGames::Lite::ReceiverGroup::ALL);
	m_pClient->opRaiseEvent(false, data, 51, option);
}

void PhotonListner_Solaseado::MoveStopFinish(FVector Loc)
{
	Hashtable data;
	float LocX = Loc.X;
	float LocY = Loc.Y;
	float LocZ = Loc.Z;
	float coords[] = { static_cast<float>(LocX),static_cast<float>(LocY),static_cast<float>(LocZ) };
	data.put((nByte)1, coords, 3);

	RaiseEventOptions option;
	option.setReceiverGroup(ExitGames::Lite::ReceiverGroup::ALL);

	m_pClient->opRaiseEvent(false, data, 52, option);
}

//NPC 업데이트
void PhotonListner_Solaseado::UpdateNPC(TArray<FString> aID, TArray<FVector> aLoc)
{
	MutableRoom& CheckRoom = m_pClient->getCurrentlyJoinedRoom();
	Hashtable data;

	int n = aID.Num();

	int npcSize[] = { static_cast<int>(n) };
	{
		FString str = "NPCSize";
		data.put(TCHAR_TO_UTF8(*str), npcSize, 1);
	}

	for (int i = 1; i <= n; ++i)
	{
		float coords[] = { static_cast<float>(aLoc[i-1].X),static_cast<float>(aLoc[i-1].Y),static_cast<float>(aLoc[i-1].Z) };
		data.put(TCHAR_TO_UTF8(*(aID[i-1])), coords, 3);
	}

	//UE_LOG(LogTemp, Log, TEXT("// PhotonListner_Solaseado::UpdateNPC"));
	CheckRoom.addCustomProperties(data);
}


//NPC 위치 이동
void PhotonListner_Solaseado::SetNPCMove(FString sName, FVector vLoc)
{
	MutableRoom& CheckRoom = m_pClient->getCurrentlyJoinedRoom();
	Hashtable data;

	float coords[] = { static_cast<float>(vLoc.X),static_cast<float>(vLoc.Y),static_cast<float>(vLoc.Z) };
	data.put(TCHAR_TO_UTF8(*sName), coords, 3);

	CheckRoom.addCustomProperties(data);

	//UE_LOG(LogTemp, Log, TEXT("//  PhotonListner_Solaseado::SetNPCMove"));
}


