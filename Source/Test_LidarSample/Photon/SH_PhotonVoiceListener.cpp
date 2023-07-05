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
static const bool DEBUG_ECHO_MODE_INIT = false; // initial state of local voice debug echo mode (streaming back to client via server)


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
	static_cast<void>(opaque);
	RemoveInfo* ri = static_cast<RemoveInfo*>(opaque);
	ri->photonLib->remoteVoiceRemoveCallback(ri->out);
	delete ri;
}

//사운드 출력부 제거 콜백
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

//여기서 플레이어 접속 콜백
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
	

	//제거 콜백 세팅
	options.setRemoteVoiceRemoveAction(new RemoveInfo(this, out), remoteVoiceRemoveCallback);

	options.setOutput(out->getPlayer(), frameDataCallback);
}

void SH_PhotonVoiceListener::JoinRoom()
{
	RoomOptions opt;
	if (DIRECT)
		opt.setDirectMode(DirectMode::ALL_TO_ALL);
	opt.setPublishUserID(true);
	opt.setMaxPlayers(0);


	mLoadBalancingClient.opJoinOrCreateRoom(v_RoomName, opt);
	mState = State::JOINING;
	
}

// Client(LoadBalancing::Listener& listener, const Common::JString& applicationID, const Common::JString& appVersion, const ClientConstructOptions& clientConstructOptions=ClientConstructOptions());
SH_PhotonVoiceListener::SH_PhotonVoiceListener(Common::JString const& appID, Common::JString const& appVersion, IAudioInFactory* audioInFactory, IAudioOutFactory* audioOutFactory, SH_PhotonVoiceBasic* PhotonVoiceBasic)
#ifdef _EG_MS_COMPILER
#	pragma warning(push)
#	pragma warning(disable:4355)
#endif
	: mState(State::INITIALIZED)
	, mSendCount(0)
	, mReceiveCount(0)
	, mVoicesCreated(false)
	, mpAudioInFactory(audioInFactory)
	, mpAudioOutFactory(audioOutFactory)
	, mBasic(PhotonVoiceBasic)
	, mLoadBalancingClient(*this, appID, appVersion) //  ConnectionProtocol::UDP, true, RegionSelectionMode::SELECT

#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
{
	// 주어진 LoadBalancing 클라이언트를 기반으로 VoiceClient에 대한 전송을 제공합니다.
	// VoiceClient 생성 및 유지
	mpTransport = new LoadBalancingTransport(mLoadBalancingClient, *this, DIRECT);
	//음성 클라이언트는 IVoiceTransport를 통해 네트워크의 다른 클라이언트와 상호 작용합니다.
	mpVoiceClient = new VoiceClient(mpTransport);
	mpVoiceClient->setOnRemoteVoiceInfoAction(this, remoteVoiceInfoCallback);//여기서 네트워크 접속하는 플레이어 데이터 콜백함수 세팅

	mpTransport->setDebugOutputLevel(DEBUG_RELEASE(Common::DebugLevel::INFO, Common::DebugLevel::INFO)); // all instances of VoiceClient that use mpTransport //mpTransport를 사용하는 VoiceClient의 모든 인스턴스
	mLoadBalancingClient.setDebugOutputLevel(DEBUG_RELEASE(Common::DebugLevel::INFO, Common::DebugLevel::WARNINGS)); // that instance of LoadBalancingClient and its implementation details // 해당 LoadBalancingClient 인스턴스 및 해당 구현 세부 정보
	mLogger.setListener(*this);
	mLogger.setDebugOutputLevel(DEBUG_RELEASE(Common::DebugLevel::INFO, Common::DebugLevel::WARNINGS)); // this 




	Common::Base::setListener(this);
	Common::Base::setDebugOutputLevel(DEBUG_RELEASE(Common::DebugLevel::INFO, Common::DebugLevel::WARNINGS)); // all classes that inherit from Base
}

SH_PhotonVoiceListener::~SH_PhotonVoiceListener(void)
{
	for (unsigned int i = 0; i < mAudioSources.size(); i++)
	{
		delete mAudioSources[i];
	}
	for (unsigned int i = 0; i < mLocalVoices.size(); i++)
	{
		mLocalVoices[i]->removeSelf();
	}

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
		//생성은 아이디를 받고 수동으로 해준다.
		// 
		//connect(L"TestPhotonVoice");
		//mState = State::CONNECTING;
		break;
	case State::CONNECTED:
	{
		
	}
	break;
	case State::JOINED:
	{
		//			sendData();
		isconnected = true;
		break;
	}
	case State::RECEIVED_DATA:
		//mLoadBalancingClient.opLeaveRoom();
		mState = State::LEAVING;
		break;
	case State::LEFT:
		isconnected = false;
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

	//플레이어마다 업데이트 해준다.
	for (unsigned int i = 0; i < mAudioPlayers.size(); i++)
	{
		mAudioPlayers[i]->service();
		//UE_LOG(LogTemp, Error, TEXT("Update player %d"),i);
	}
}

void SH_PhotonVoiceListener::connect(const Common::JString& name)
{
	// + GETTIMEMS()
	mLoadBalancingClient.connect(ConnectOptions().setAuthenticationValues(AuthenticationValues().setUserID(name)));
	mState = State::CONNECTING;
	
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
	//Console::get().debugReturn(debugLevel, string);
}

void SH_PhotonVoiceListener::connectionErrorReturn(int errorCode)
{
	EGLOG(Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	//Console::get().writeLine(Common::JString(L"received connection error ") + errorCode);
	mState = State::DISCONNECTED;
}

void SH_PhotonVoiceListener::clientErrorReturn(int errorCode)
{
	EGLOG(Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	//Console::get().writeLine(Common::JString(L"received error ") + errorCode + L" from client");
}

void SH_PhotonVoiceListener::warningReturn(int warningCode)
{
	EGLOG(Common::DebugLevel::WARNINGS, L"code: %d", warningCode);
	//Console::get().writeLine(Common::JString(L"received warning ") + warningCode + L" from client");
}

void SH_PhotonVoiceListener::serverErrorReturn(int errorCode)
{
	EGLOG(Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	//Console::get().writeLine(Common::JString(L"received error ") + errorCode + " from server");
}

//제대로 player 정보값을 읽지 못함. 룸옵션에서 setPublishUserID()로 공유가 가능하게 풀어줘야 한다.
void SH_PhotonVoiceListener::joinRoomEventAction(int playerNr, const Common::JVector<int>& playernrs, const Player& player)
{
	LoadBalancingListener::joinRoomEventAction(playerNr, playernrs, player);

	EGLOG(Common::DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
	//Console::get().writeLine(L"");
	Console::get().writeLine(Common::JString(L"player ") + playerNr + L" " + player.getName() + L" has joined the game");

	//최초로 들어갔을때만 저장한다.
	if (!mVoicesCreated)
	{
		//ToneAudioPusher<short>* audioSource0=new ToneAudioPusher<short>(440, 100, 16000, 1);
		//VoiceInfo i0=VoiceInfo::createAudioOpus(16000, audioSource0->getChannels(), 10000, 30000);
		//LocalVoice* v0 = mpVoiceClient->createLocalVoiceAudioFromSource(i0, audioSource0, 0);
		//v0->setDebugEchoMode(DEBUG_ECHO_MODE_INIT);
		//mAudioSources.push_back(audioSource0);
		//mLocalVoices.push_back(v0);

		//IAudioPusher-> AudioIn
		IAudioPusher<short>* audioSource2 = new AudioIn(mpAudioInFactory);

		
		VoiceInfo i2 = VoiceInfo::createAudioOpus(16000, audioSource2->getChannels(), 20000, 30000);
		//CreateLocalVoiceAudioFromSource : 발신 오디오 스트림을 생성하고 오디오 소스 데이터를 소비하기 위한 절차를 추가한다.처리 파이프라인과 반환되는 스트림 핸들러에 오디오 관련기능 추가한다.
		// default or user's decoder
		LocalVoiceAudio<short>* v2 = mpVoiceClient->createLocalVoiceAudioFromSource(i2, audioSource2, 0);
		v2->setDebugEchoMode(DEBUG_ECHO_MODE_INIT);
		v2->getVoiceDetector()->setOn(false);
		mAudioSources.push_back(audioSource2);
		mLocalVoices.push_back(v2);
		
		//Cast<AudioIn>(audioSource2)->MuteInputSound(true);
		v2->setTransmitEnabled(true);//이걸로 오디오 활성화를 조절할 수 있음
		mVoicesCreated = true;
		{
			FString str = UTF8_TO_TCHAR(v2->getName().UTF8Representation().cstr());
			UE_LOG(LogTemp, Log, TEXT("//joinRoomEventAction Init // v2 Player Name :: %s , "), *str);
		}
	}

	{
		FString str2 = UTF8_TO_TCHAR(player.getUserID().UTF8Representation().cstr());
		UE_LOG(LogTemp, Log, TEXT("//joinRoomEventAction End//UserID :: %s , Number :: %d"), *str2, playerNr);
	}

	mCharacterInfo.put(player.getUserID(), playerNr);
	
	//찾는 플레이어 정보
	if (mCharacterInfo.getValue((player.getUserID())))
	{
		int32 data =  ((Common::ValueObject<int32>*)mCharacterInfo.getValue(player.getUserID()))->getDataCopy();

		UE_LOG(LogTemp, Log, TEXT("//joinRoomEventAction End//UserID Number :: %d"), data);
	}
}

void SH_PhotonVoiceListener::leaveRoomEventAction(int playerNr, bool isInactive)
{
	LoadBalancingListener::leaveRoomEventAction(playerNr, isInactive);

	EGLOG(Common::DebugLevel::INFO, L"");
	//Console::get().writeLine(L"");
	//Console::get().writeLine(Common::JString(L"player ") + playerNr + L" has left the game");
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

	mBasic->Voice_ConnectComplete();
}

void SH_PhotonVoiceListener::disconnectReturn(void)
{
	LoadBalancingListener::disconnectReturn();

	EGLOG(Common::DebugLevel::INFO, L"");
	//Console::get().writeLine(L"disconnected");
	mState = State::DISCONNECTED;
}

void SH_PhotonVoiceListener::createRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	LoadBalancingListener::createRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		//Console::get().writeLine(L"opCreateRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mState = State::JOINED;
	isconnected = true;
}

void SH_PhotonVoiceListener::joinOrCreateRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	
	if (errorCode)
	{
		mState = State::CONNECTED;
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("//joinOrCreateRoomReturn// voicde id :: %d"), localPlayerNr);

	mState = State::JOINED;
	IsChanging = false;
}

void SH_PhotonVoiceListener::joinRandomOrCreateRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	LoadBalancingListener::joinRandomOrCreateRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		//Console::get().writeLine(L"opJoinRandomOrCreateRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	//Console::get().writeLine(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been entered");
	//Console::get().writeLine(L"regularly sending dummy events now");
	mState = State::JOINED;

}


void SH_PhotonVoiceListener::joinRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	LoadBalancingListener::joinRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		//Console::get().writeLine(L"opJoinRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}
	EGLOG(Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	//Console::get().writeLine(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been successfully joined");
	//Console::get().writeLine(L"regularly sending dummy events now");

	UE_LOG(LogTemp, Log, TEXT("//joinRoomReturn// voicde id :: %d"), localPlayerNr);

	mState = State::JOINED;
}

void SH_PhotonVoiceListener::joinRandomRoomReturn(int localPlayerNr, const Common::Hashtable& gameProperties, const Common::Hashtable& playerProperties, int errorCode, const Common::JString& errorString)
{
	LoadBalancingListener::joinRandomRoomReturn(localPlayerNr, gameProperties, playerProperties, errorCode, errorString);

	EGLOG(Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		//Console::get().writeLine(L"opJoinRandomRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	//Console::get().writeLine(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been successfully joined");
	//Console::get().writeLine(L"regularly sending dummy events now");
	mState = State::JOINED;
}

void SH_PhotonVoiceListener::leaveRoomReturn(int errorCode, const Common::JString& errorString)
{
	//LoadBalancingListener::leaveRoomReturn(errorCode, errorString);
	if (mVoicesCreated)
	{
		mVoicesCreated = false;
		for (unsigned int i = 0; i < mAudioSources.size(); i++)
		{
			delete mAudioSources[i];
		}
		mAudioSources.clear();
		for (unsigned int i = 0; i < mLocalVoices.size(); i++)
		{
			mLocalVoices[i]->removeSelf();
		}
		mLocalVoices.clear();
	}


	mState = State::LEFT;
	if (IsChanging)
	{
		JoinRoom();
		return;
	}
	else if (errorCode)
	{
		mState = State::DISCONNECTING;
		return;
	}
}

void SH_PhotonVoiceListener::joinLobbyReturn(void)
{
	EGLOG(Common::DebugLevel::INFO, L"");
	//Console::get().writeLine(L"joined lobby");
}

void SH_PhotonVoiceListener::leaveLobbyReturn(void)
{
	EGLOG(Common::DebugLevel::INFO, L"");
	//Console::get().writeLine(L"left lobby");
}

void SH_PhotonVoiceListener::onAvailableRegions(const Common::JVector<Common::JString>& /*availableRegions*/, const Common::JVector<Common::JString>& /*availableRegionServers*/)
{
	mLoadBalancingClient.selectRegion(L"kr");
}

