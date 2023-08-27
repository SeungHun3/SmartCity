// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Common-cpp/inc/Common.h"

class FString;

class SH_PhotonBasic
{

public:
	virtual ~SH_PhotonBasic() {};

	virtual void ErrorCheckMessage(const FString& message, int error) {}
	
	virtual void InitPlayers(void) {}
	virtual void AddPlayers(int playerNr, const ExitGames::Common::JString& playerName, bool local, const ExitGames::Common::Hashtable& Custom) {}
	virtual void RemovePlayer(int playerNr) {}

	// Connect
	virtual void ConnectComplete(void) {}

	// Photon Region
	virtual void setRegion(ExitGames::LoadBalancing::Client* Client) {}

	// Photon Room 
	virtual void CreateChannelComplete(const ExitGames::Common::JString& map, const ExitGames::Common::JString& channel) {}
	virtual void CreateRoomComplete(const ExitGames::Common::JString& map) {}
	virtual void JoinRoomComplete(const ExitGames::Common::JString& map, const ExitGames::Common::JString& channel) {}
	virtual void JoinOrCreateComplete(const FString& RoomFullName) {}

	virtual void LeaveRoomComplete(void) {}
	virtual void CurrentRoomInfo(const ExitGames::Common::JString& Name, nByte Count, nByte MaxCount) {}

	// anim // Character info
	virtual void updatePlayerProperties(int playerNr, const ExitGames::Common::Hashtable& changes) {}

	// change room Property 
	virtual void updateRoomProperties(const ExitGames::Common::Hashtable& changes) {}
	virtual void UpdateRoomList(const TMap<int, int>& Channel_Count) {}
	// Game
	virtual void GetMoveAndRotation(int playerNr, int vX, int vY, int vZ,int vYaw) {}
	virtual void GetPlayerAnim(int playerNr, int Anim) {}

	// Quiz Event
	virtual void getEventPause(bool ev) {}
	virtual void updateLocalPlayerPosion() {}

	// 이동
	virtual void UpdateMove(int PlayerNum, FVector Loc) {}
	virtual void UpdateStop(int PlayerNum) {}
	virtual void UpdateStopFinished(int PlayerNum, FVector Loc) {}

	//NPC
	virtual void InitNpc() {}
	virtual void AddNpc(const FString& NpcId, FVector Loc, FVector Rot, FVector Scale) {}
	virtual void ClearNpc() {}

	//친구
	virtual void RecvFriendRequest(int PlayerNum, const FString& FriendPlayFabID) {}
};
