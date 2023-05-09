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
	virtual void setRegion(void) {}

	// Photon Room 
	virtual void CreateChannelComplete(const ExitGames::Common::JString& map, const ExitGames::Common::JString& channel) {}
	virtual void CreateRoomComplete(const ExitGames::Common::JString& map) {}
	virtual void JoinRoomComplete(const ExitGames::Common::JString& map, const ExitGames::Common::JString& channel) {}
	virtual void JoinOrCreateComplete() {}

	virtual void LeaveRoomComplete(void) {}
	virtual void CurrentRoomInfo(const ExitGames::Common::JString& Name, nByte Count, nByte MaxCount) {}

	// anim // Character info
	virtual void updatePlayerProperties(int playerNr, const ExitGames::Common::Hashtable& changes) {}

	// change room Property 
	virtual void updateRoomProperties(const ExitGames::Common::Hashtable& changes) {}
	
	// Game
	virtual void getTextMessage(int playerNr, const ExitGames::Common::JString& Message, const ExitGames::Common::JString& Type) {}
	virtual void GetMovePlayerRotation(int playerNr, float fX) {}
	virtual void GetMovePlayer(int playerNr, int vX, int vY, int vZ) {}

	// Quiz Event
	virtual void getEventPause(bool ev) {}
	virtual void updateLocalPlayerPosion() {}
};
