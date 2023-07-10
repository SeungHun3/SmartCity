// Fill out your copyright notice in the Description page of Project Settings.


#include "SH_PhotonChatListener.h"
#include "Common-cpp/inc/Common.h"
#include "PhotonListner_Solaseado.h"

#include "Console.h"

using namespace ExitGames::LoadBalancing;

SH_PhotonChatListener::SH_PhotonChatListener(SH_PhotonChatBasic* basicView) : m_pView(basicView)
{

}

SH_PhotonChatListener::~SH_PhotonChatListener()
{
	
}

////////////////////////////////////////////////////////////////////////
// Debuging 
void SH_PhotonChatListener::debugReturn(int debugLevel, const Common::JString& string)
{
	Console::get().writeLine(Common::JString("// PhotonChat debugReturn :: ") + debugLevel + Common::JString(" // ") + string);
}

void SH_PhotonChatListener::onStateChange(int state)
{
	Console::get().writeLine(Common::JString("// PhotonChat onStateChange :: ") + state);
}

void SH_PhotonChatListener::connectionErrorReturn(int errorCode)
{
	Console::get().writeLine(Common::JString("// PhotonChat connectionErrorReturn :: ") + errorCode);
}

void SH_PhotonChatListener::clientErrorReturn(int errorCode)
{
	Console::get().writeLine(Common::JString("// PhotonChat clientErrorReturn :: ") + errorCode);
}

void SH_PhotonChatListener::warningReturn(int warningCode)
{
	Console::get().writeLine(Common::JString("// PhotonChat warningReturn :: " )+ warningCode);
}

void SH_PhotonChatListener::serverErrorReturn(int errorCode)
{
	Console::get().writeLine(Common::JString("// PhotonChat serverErrorReturn :: ")+ errorCode);
}

////////////////////////////////////////////////////////////////////////
// callbacks for operations on server
void SH_PhotonChatListener::connectReturn(int errorCode, const Common::JString& errorString)
{
	//UE_LOG(LogTemp, Log, TEXT("//Chat connectReturn::"));
	Console::get().writeLine(Common::JString("// PhotonChat errorCode ConnectReturn :: ") + errorCode);
	if (errorCode == ErrorCode::OK)
	{
		// 기본 채널 접속
		// Common::JVector<Common::JString> channels;
		// channels.addElement(L"TEST");
		// m_pChatClient->opSubscribe(channels);
		
		// AActor_PhotonChat function 

		//채팅 채널에 성공적으로 접속했다.
		m_pView->Chat_ConnectComplete();
	}
	else
	{
		Console::get().writeLine(Common::JString("// // errorCode Message :: ") + errorString);
	}

}

//접속이 끊김
void SH_PhotonChatListener::disconnectReturn(void)
{
	Console::get().writeLine(Common::JString("// PhotonChat :: disconnectReturn "));
}


//구독 리턴
void SH_PhotonChatListener::subscribeReturn(const Common::JVector<Common::JString>& channels, const Common::JVector<bool>& results)
{
	// 채팅 시스템은 정해진 구독 채널 안에서 활용됨.
	// 전체, 지역, 그룹 .. 
	Console::get().writeLine(Common::JString("// PhotonChat :: subscribeReturn"));
	int size = channels.getSize();
	for (int i = 0; i < size; i++)
	{
		Console::get().writeLine(Common::JString("// // Channels :: ") + channels[i] + Common::JString(" / result :: ") + results[i]);
		if (results[i])
		{
			m_pView->Chat_AddSubscribe(UTF8_TO_TCHAR(channels[i].UTF8Representation().cstr()));
		}
	}
}

void SH_PhotonChatListener::unsubscribeReturn(const Common::JVector<Common::JString>& channels)
{
	Console::get().writeLine(Common::JString("// PhotonChat :: unsubscribeReturn"));
	for (unsigned int i = 0; i < channels.getSize(); i++)
	{
		Console::get().writeLine(Common::JString("// // Channels :: ") + channels[i]);
		m_pView->Chat_RemoveSubscribe(UTF8_TO_TCHAR(channels[i].UTF8Representation().cstr()));
	}
	
}

void SH_PhotonChatListener::onStatusUpdate(const Common::JString& user, int status, bool gotMessage, const Common::Object& message)
{
	Console::get().writeLine(Common::JString("// PhotonChat :: onStatusUpdate ") + user);
}

//일반 메시지 수신부
void SH_PhotonChatListener::onGetMessages(const Common::JString& channelName, const Common::JVector<Common::JString>& senders, const Common::JVector<Common::Object>& messages)
{
	Console::get().writeLine(Common::JString("// PhotonChat :: onGetMessages ChannelName " )+ channelName);
	int size = senders.getSize();

	for (int i = 0; i < size; i++)
	{
		if (messages[i].getType() == ExitGames::Common::TypeCode::STRING)
		{
			ExitGames::Common::JString Check = ExitGames::Common::ValueObject<ExitGames::Common::JString>(messages[i]).getDataCopy();
			// JString* data = ((ValueObject<JString*>*)obj)->getDataCopy();
			// Console::get().writeLine(Common::JString("// // Senders :: ") + senders[i] + Common::JString("// Message :: ") + Check);
			m_pView->Chat_getMessageEvent(FString(UTF8_TO_TCHAR(channelName.UTF8Representation().cstr())) , FString(UTF8_TO_TCHAR(senders[i].UTF8Representation().cstr())), FString(UTF8_TO_TCHAR(Check.UTF8Representation().cstr())));
		}
		else
		{
			Console::get().writeLine(Common::JString("// // Senders :: ") + senders[i] + Common::JString("// Not String  "));
		}
	}
}

//귓속말 수신부
void SH_PhotonChatListener::onPrivateMessage(const Common::JString& sender, const Common::Object& message, const Common::JString& channelName)
{
	Console::get().writeLine(Common::JString("// onPrivateMessage sender :: ") + sender);
	Console::get().writeLine(Common::JString("// // message		:: ") );
	Console::get().writeLine(Common::JString("// // channelName :: ") + channelName);


}

void SH_PhotonChatListener::onReceiveBroadcastMessage(const Common::JString& channelName, nByte* inBuff, int inBuffBodyLength)
{
	Console::get().writeLine(Common::JString("// PhotonChat :: onReceiveBroadcastMessage channelName ") + channelName);
}


