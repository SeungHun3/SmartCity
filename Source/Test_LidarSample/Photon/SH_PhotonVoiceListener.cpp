// Fill out your copyright notice in the Description page of Project Settings.


#include "SH_PhotonVoiceListener.h"

#include <algorithm>
#include <vector>
#include "limits.h"
#include "LoadBalancing-cpp/inc/Enums/DirectMode.h"
#include "PhotonVoice-cpp/inc/Transport/LoadBalancingListener.h"
#include "PhotonVoice-cpp/inc/Transport/LoadBalancingTransport.h"
#include "PhotonVoice-cpp/inc/RemoteVoiceOptions.h"
#include "PhotonVoice-cpp/inc/AudioGen.h"
#include "PhotonVoice-cpp/inc/OpusCodec.h"
#include "../Actor_PhotonAudioIn.h"
#include "Console.h"

using namespace ExitGames;
//using namespace ExitGames::Common; // outcommented to avoid symbol clashes between UE_LOG and Common::JString
using namespace ExitGames::Common::Helpers;
using namespace ExitGames::Common::MemoryManagement;
using namespace ExitGames::LoadBalancing;
using namespace ExitGames::Photon;
using namespace ExitGames::Voice;
using namespace ExitGames::Voice::AudioUtil;

static const bool DIRECT = false; // send audio peer to peer (debug echo mode is not supported)
static const bool DEBUG_ECHO_MODE_INIT = true; // initial state of local voice debug echo mode (streaming back to client via server)

static Common::JString gameName = L"Test_VoiceBasics";

static const int MAX_SENDCOUNT = 100;

static void frameDataCallback(void* opaque, const Buffer<short>& frame)
{
	IAudioOut<short>* audioOut = static_cast<IAudioOut<short>*>(opaque);
	audioOut->push(frame);
}

struct RemoveInfo
{
	RemoveInfo(SH_PhotonVoiceListener* photonLib, AActor_PhotonAudioOut* out)
		: photonLib(photonLib), out(out)
	{}
	SH_PhotonVoiceListener* photonLib;
	AActor_PhotonAudioOut* out;
};

void SH_PhotonVoiceListener::remoteVoiceRemoveCallback(void* opaque)
{
	RemoveInfo* ri = static_cast<RemoveInfo*>(opaque);
	ri->photonLib->remoteVoiceRemoveCallback(ri->out);
	delete ri;
}

void SH_PhotonVoiceListener::remoteVoiceRemoveCallback(AActor_PhotonAudioOut* out)
{
	out->getPlayer()->stop();
	mAudioPlayers.erase(std::remove(mAudioPlayers.begin(), mAudioPlayers.end(), out->getPlayer()), mAudioPlayers.end());
	out->finit();
	mpAudioOutFactory->destroyAudioOut(out);
}

void SH_PhotonVoiceListener::remoteVoiceInfoCallback(void* opaque, int channelId, int playerId, nByte voiceId, VoiceInfo const& voiceInfo, RemoteVoiceOptions& options)
{
	static_cast<SH_PhotonVoiceListener*>(opaque)->remoteVoiceInfoCallback(channelId, playerId, voiceId, voiceInfo, options);
}

void SH_PhotonVoiceListener::remoteVoiceInfoCallback(int channelId, int playerId, nByte voiceId, VoiceInfo const& voiceInfo, RemoteVoiceOptions& options)
{
	AudioOutDelayConfig delayConfig(200, 500, 1000, 5);
	AActor_PhotonAudioOut* out = mpAudioOutFactory->createAudioOut();
	out->init(
		*mpTransport
		, delayConfig
		, Common::JString(L"Audio Player p#") + playerId + L", v#" + voiceId
		, true);

	IAudioOut<short>* p = out->getPlayer();
	p->start(voiceInfo.getSamplingRate(), voiceInfo.getChannels(), voiceInfo.getFrameDurationSamples());
	mAudioPlayers.push_back(out->getPlayer());
	options.setRemoteVoiceRemoveAction(new RemoveInfo(this, out), remoteVoiceRemoveCallback);

	options.setOutput(out->getPlayer(), frameDataCallback);
}
// Client(LoadBalancing::Listener& listener, const Common::JString& applicationID, const Common::JString& appVersion, const ClientConstructOptions& clientConstructOptions=ClientConstructOptions());
SH_PhotonVoiceListener::SH_PhotonVoiceListener(Common::JString const& appID, Common::JString const& appVersion, IAudioInFactory* audioInFactory, IAudioOutFactory* audioOutFactory)
#ifdef _EG_MS_COMPILER
#	pragma warning(push)
#	pragma warning(disable:4355)
#endif
	: mState(State::INITIALIZED)
	, mLoadBalancingClient(*this, appID, appVersion) //  ConnectionProtocol::UDP, true, RegionSelectionMode::SELECT
	, mSendCount(0)
	, mReceiveCount(0)
	, mVoicesCreated(false)
	, mpAudioInFactory(audioInFactory)
	, mpAudioOutFactory(audioOutFactory)
#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
{
	mpTransport = new LoadBalancingTransport(mLoadBalancingClient, *this, DIRECT);
	mpVoiceClient = new VoiceClient(mpTransport);
	mpVoiceClient->setOnRemoteVoiceInfoAction(this, remoteVoiceInfoCallback);

	mpTransport->setDebugOutputLevel(DEBUG_RELEASE(Common::DebugLevel::INFO, Common::DebugLevel::INFO)); // all instances of VoiceClient that use mpTransport
	mLoadBalancingClient.setDebugOutputLevel(DEBUG_RELEASE(Common::DebugLevel::INFO, Common::DebugLevel::WARNINGS)); // that instance of LoadBalancingClient and its implementation details
	mLogger.setListener(*this);
	mLogger.setDebugOutputLevel(DEBUG_RELEASE(Common::DebugLevel::INFO, Common::DebugLevel::WARNINGS)); // this class
	Common::Base::setListener(this);
	Common::Base::setDebugOutputLevel(DEBUG_RELEASE(Common::DebugLevel::INFO, Common::DebugLevel::WARNINGS)); // all classes that inherit from Base
}

SH_PhotonVoiceListener::~SH_PhotonVoiceListener(void)
{
	for (unsigned int i = 0; i < mAudioSources.size(); i++)
		delete mAudioSources[i];
	for (unsigned int i = 0; i < mLocalVoices.size(); i++)
		mLocalVoices[i]->removeSelf();
	// sources and players are AActor's destroyed automatically
	delete mpVoiceClient;
	delete mpTransport;
}

void SH_PhotonVoiceListener::update(void)
{
	// 테스트 자동화 기능 
	switch (mState)
	{
	case State::INITIALIZED:
		connect(L"TestPhotonVoice");
		mState = State::CONNECTING;
		break;
	case State::CONNECTED:
	{
		RoomOptions opt;
		if (DIRECT)
			opt.setDirectMode(DirectMode::ALL_TO_ALL);
		mLoadBalancingClient.opJoinOrCreateRoom(gameName, opt);
		mState = State::JOINING;
	}
	break;
	case State::JOINED:
		//			sendData();
		break;
	case State::RECEIVED_DATA:
		mLoadBalancingClient.opLeaveRoom();
		mState = State::LEAVING;
		break;
	case State::LEFT:
		mLoadBalancingClient.disconnect();
		mState = State::DISCONNECTING;
		break;
	case State::DISCONNECTED:
		//			mState = State::INITIALIZED;
		break;
	default:
		break;
	}

	mLoadBalancingClient.service();
	mpTransport->service();
	mpVoiceClient->service();

	for (unsigned int i = 0; i < mAudioPlayers.size(); i++)
		mAudioPlayers[i]->service();
}

void SH_PhotonVoiceListener::connect(const Common::JString& name)
{
	mLoadBalancingClient.connect(ConnectOptions().setAuthenticationValues(AuthenticationValues().setUserID(name)));
}

void SH_PhotonVoiceListener::disconnect(void)
{
	mLoadBalancingClient.disconnect();
}

void SH_PhotonVoiceListener::toggleEcho(void)
{
	for (unsigned int i = 0; i < mLocalVoices.size(); i++)
	{
		mLocalVoices[i]->setDebugEchoMode(!mLocalVoices[i]->getDebugEchoMode());
		Console::get().writeLine(mLocalVoices[i]->getName() + L" echo: " + (mLocalVoices[i]->getDebugEchoMode() ? L"ON" : L"OFF"));
	}
}

Common::JString SH_PhotonVoiceListener::getStateString(void)
{
	switch (mState)
	{
	case State::INITIALIZED:
		return L"disconnected";
	case State::CONNECTING:
		return L"connecting";
	case State::CONNECTED:
		return L"connected";
	case State::JOINING:
		return L"joining";
	case State::JOINED:
		return Common::JString(L"ingame\nsent event Nr. ") + mSendCount + L"\nreceived event Nr. " + mReceiveCount;
	case State::SENT_DATA:
		return Common::JString(L"sending completed") + L"\nreceived event Nr. " + mReceiveCount;
	case State::RECEIVED_DATA:
		return L"receiving completed";
	case State::LEAVING:
		return L"leaving";
	case State::LEFT:
		return L"left";
	case State::DISCONNECTING:
		return L"disconnecting";
	case State::DISCONNECTED:
		return L"disconnected";
	default:
		return L"unknown state";
	}
}

void SH_PhotonVoiceListener::sendData(void)
{
	Common::Hashtable event;
	event.put(static_cast<nByte>(0), ++mSendCount);
	// send to ourselves only
	int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
	mLoadBalancingClient.opRaiseEvent(true, event, 0, RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
	if (mSendCount >= MAX_SENDCOUNT)
		mState = State::SENT_DATA;
}

void SH_PhotonVoiceListener::debugReturn(int debugLevel, const Common::JString& string)
{
	switch (debugLevel)
	{
	case Common::DebugLevel::ERRORS:
		UE_LOG(LogTemp, Error, TEXT("%hs"), string.UTF8Representation().cstr());
		break;
	case Common::DebugLevel::WARNINGS:
		UE_LOG(LogTemp, Warning, TEXT("%hs"), string.UTF8Representation().cstr());
		break;
	default:
		UE_LOG(LogTemp, Log, TEXT("%hs"), string.UTF8Representation().cstr());
		break;
	}
	Console::get().debugReturn(debugLevel, string);
}

void SH_PhotonVoiceListener::connectionErrorReturn(int errorCode)
{
	EGLOG(Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	Console::get().writeLine(Common::JString(L"received connection error ") + errorCode);
	mState = State::DISCONNECTED;
}

void SH_PhotonVoiceListener::clientErrorReturn(int errorCode)
{
	EGLOG(Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	Console::get().writeLine(Common::JString(L"received error ") + errorCode + L" from client");
}

void SH_PhotonVoiceListener::warningReturn(int warningCode)
{
	EGLOG(Common::DebugLevel::WARNINGS, L"code: %d", warningCode);
	Console::get().writeLine(Common::JString(L"received warning ") + warningCode + L" from client");
}

void SH_PhotonVoiceListener::serverErrorReturn(int errorCode)
{
	EGLOG(Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	Console::get().writeLine(Common::JString(L"received error ") + errorCode + " from server");
}

void SH_PhotonVoiceListener::joinRoomEventAction(int playerNr, const Common::JVector<int>& playernrs, const Player& player)
{
	LoadBalancingListener::joinRoomEventAction(playerNr, playernrs, player);

	EGLOG(Common::DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
	Console::get().writeLine(L"");
	Console::get().writeLine(Common::JString(L"player ") + playerNr + L" " + player.getName() + L" has joined the game");

	if (!mVoicesCreated)
	{
		//ToneAudioPusher<short>* audioSource0=new ToneAudioPusher<short>(440, 100, 16000, 1);
		//VoiceInfo i0=VoiceInfo::createAudioOpus(16000, audioSource0->getChannels(), 10000, 30000);
		//LocalVoice* v0 = mpVoiceClient->createLocalVoiceAudioFromSource(i0, audioSource0, 0);
		//v0->setDebugEchoMode(DEBUG_ECHO_MODE_INIT);
		//mAudioSources.push_back(audioSource0);
		//mLocalVoices.push_back(v0);

		IAudioPusher<short>* audioSource2 = new AudioIn(mpAudioInFactory);
		VoiceInfo i2 = VoiceInfo::createAudioOpus(16000, audioSource2->getChannels(), 20000, 30000);
		// default or user's decoder
		LocalVoiceAudio<short>* v2 = mpVoiceClient->createLocalVoiceAudioFromSource(i2, audioSource2, 0);
		v2->setDebugEchoMode(DEBUG_ECHO_MODE_INIT);
		v2->getVoiceDetector()->setOn(false);
		mAudioSources.push_back(audioSource2);
		mLocalVoices.push_back(v2);

		mVoicesCreated = true;
	}
	EGLOG(Common::DebugLevel::INFO, L"\n\n\n                         Press 'e' to toggle debug echo mode\n\n");
}

void SH_PhotonVoiceListener::leaveRoomEventAction(int playerNr, bool isInactive)
{
	LoadBalancingListener::leaveRoomEventAction(playerNr, isInactive);

	EGLOG(Common::DebugLevel::INFO, L"");
	Console::get().writeLine(L"");
	Console::get().writeLine(Common::JString(L"player ") + playerNr + L" has left the game");
}

void SH_PhotonVoiceListener::customEventAction(int playerNr, nByte eventCode, const Common::Object& eventContentObj)
{
	LoadBalancingListener::customEventAction(playerNr, eventCode, eventContentObj);

	Common::Hashtable eventContent = Common::ValueObject<Common::Hashtable>(eventContentObj).getDataCopy();
	switch (eventCode)
	{
	case 0:
		if (eventContent.getValue((nByte)0))
			mReceiveCount = ((Common::ValueObject<int64>*)(eventContent.getValue((nByte)0)))->getDataCopy();
		if (mState == State::SENT_DATA && mReceiveCount >= mSendCount)
		{
			mState = State::RECEIVED_DATA;
			mSendCount = 0;
			mReceiveCount = 0;
		}
		break;
	default:
		break;
	}

}

void SH_PhotonVoiceListener::connectReturn(int errorCode, const Common::JString& errorString, const Common::JString& region, const Common::JString& cluster)
{
	EGLOG(Common::DebugLevel::INFO, L"connected to cluster " + cluster + L" of region " + region);
	if (errorCode)
	{
		EGLOG(Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mState = State::DISCONNECTING;
		return;
	}
	Console::get().writeLine(L"connected to cluster " + cluster);
	mState = State::CONNECTED;

}

void SH_PhotonVoiceListener::disconnectReturn(void)
{
	LoadBalancingListener::disconnectReturn();

	EGLOG(Common::DebugLevel::INFO, L"");
	Console::get().writeLine(L"disconnected");
	mState = State::DISCONNECTED;
}

void SH_PhotonVoiceListener::createRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	LoadBalancingListener::createRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		Console::get().writeLine(L"opCreateRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	Console::get().writeLine(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been created");
	Console::get().writeLine(L"regularly sending dummy events now");
	mState = State::JOINED;
}

void SH_PhotonVoiceListener::joinOrCreateRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	LoadBalancingListener::joinOrCreateRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		Console::get().writeLine(L"opJoinOrCreateRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	Console::get().writeLine(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been entered");
	Console::get().writeLine(L"regularly sending dummy events now");
	mState = State::JOINED;
}

void SH_PhotonVoiceListener::joinRandomOrCreateRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	LoadBalancingListener::joinRandomOrCreateRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		Console::get().writeLine(L"opJoinRandomOrCreateRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	Console::get().writeLine(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been entered");
	Console::get().writeLine(L"regularly sending dummy events now");
	mState = State::JOINED;
}


void SH_PhotonVoiceListener::joinRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	LoadBalancingListener::joinRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		Console::get().writeLine(L"opJoinRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}
	EGLOG(Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	Console::get().writeLine(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been successfully joined");
	Console::get().writeLine(L"regularly sending dummy events now");

	mState = State::JOINED;
}

void SH_PhotonVoiceListener::joinRandomRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	LoadBalancingListener::joinRandomRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		Console::get().writeLine(L"opJoinRandomRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	Console::get().writeLine(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been successfully joined");
	Console::get().writeLine(L"regularly sending dummy events now");
	mState = State::JOINED;
}

void SH_PhotonVoiceListener::leaveRoomReturn(int errorCode, const Common::JString& errorString)
{
	LoadBalancingListener::leaveRoomReturn(errorCode, errorString);

	EGLOG(Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		Console::get().writeLine(L"opLeaveRoom() failed: " + errorString);
		mState = State::DISCONNECTING;
		return;
	}
	mState = State::LEFT;
	Console::get().writeLine(L"room has been successfully left");
}

void SH_PhotonVoiceListener::joinLobbyReturn(void)
{
	EGLOG(Common::DebugLevel::INFO, L"");
	Console::get().writeLine(L"joined lobby");
}

void SH_PhotonVoiceListener::leaveLobbyReturn(void)
{
	EGLOG(Common::DebugLevel::INFO, L"");
	Console::get().writeLine(L"left lobby");
}

void SH_PhotonVoiceListener::onAvailableRegions(const Common::JVector<Common::JString>& /*availableRegions*/, const Common::JVector<Common::JString>& /*availableRegionServers*/)
{
	mLoadBalancingClient.selectRegion(L"eu");
}

