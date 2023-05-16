// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Common-cpp/inc/Common.h"

class FString;

class SH_PhotonVoiceBasic
{

public:
	virtual ~SH_PhotonVoiceBasic() {};

	// Connect
	virtual void Voice_ConnectComplete(void) {}
};
