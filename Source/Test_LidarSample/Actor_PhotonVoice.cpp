// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_PhotonVoice.h"
#include "Photon/SH_PhotonVoiceListener.h"
#include "GameFramework/Actor.h"
#include "Photon/Console.h"
#include "Actor_PhotonAudioIn.h"

using namespace ExitGames::Voice;

#define Voice_AppID "4688d25f-b9ff-449a-a2a4-5c081f77fd40"
#define CHAT_AppID "bd96a445-11ab-4bc6-bb45-71b681b17273"

// bd96a445-11ab-4bc6-bb45-71b681b17273
AActor_PhotonVoice::AActor_PhotonVoice() : 
	serverAddress(TEXT("ns.exitgames.com")), // ns.exitgames.com // ns.photonengine.io
	AppID(TEXT(Voice_AppID)),
	appVersion(TEXT("1.0.1")),
	mpPhotonLib(NULL)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AActor_PhotonVoice::BeginPlay(void)
{
	Super::BeginPlay();
	srand(GETTIMEMS());
	mpPhotonLib = new SH_PhotonVoiceListener(ExitGames::Common::JString(TCHAR_TO_UTF8(*AppID)), ExitGames::Common::JString(TCHAR_TO_UTF8(*appVersion)), this, this);
}

void AActor_PhotonVoice::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);
	delete mpPhotonLib;
}

AActor_PhotonAudioIn* AActor_PhotonVoice::createAudioIn()
{
	return GetWorld()->SpawnActor<AActor_PhotonAudioIn>();
}

void AActor_PhotonVoice::destroyAudioIn(AActor_PhotonAudioIn* a)
{
	GetWorld()->DestroyActor(a);
}

AActor_PhotonAudioOut* AActor_PhotonVoice::createAudioOut()
{
	return GetWorld()->SpawnActor<AActor_PhotonAudioOut>();
}

void AActor_PhotonVoice::destroyAudioOut(AActor_PhotonAudioOut* a)
{
	GetWorld()->DestroyActor(a);
}


void AActor_PhotonVoice::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (mpPhotonLib)
	{
		mpPhotonLib->update();
	}
}

void AActor_PhotonVoice::Connect(const FString& Vociename)
{
	Console::get().writeLine(L"// Connecting...");
	mpPhotonLib->connect(TCHAR_TO_UTF8(*Vociename));
}

void AActor_PhotonVoice::Disconnect(void)
{
	mpPhotonLib->disconnect();
}

void AActor_PhotonVoice::ToggleEcho(void)
{
	mpPhotonLib->toggleEcho();
}

#if WITH_EDITOR
void AActor_PhotonVoice::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

	FName PropertyName = (e.Property) ? e.Property->GetFName() : NAME_None;
	//if(PropertyName == GET_MEMBER_NAME_CHECKED(AActor_PhotonVoice, automove))
	//	SetAutomove(automove);
	//else if(PropertyName == GET_MEMBER_NAME_CHECKED(AActor_PhotonVoice, useGroups))
	//	SetUseGroups(useGroups);
}
#endif

