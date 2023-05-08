// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Photon/SH_PhotonVoiceListener.h"
#include "Actor_PhotonVoice.generated.h"

UCLASS()
class TEST_LIDARSAMPLE_API AActor_PhotonVoice : public AActor, public IAudioInFactory, public IAudioOutFactory
{
	GENERATED_BODY()

protected:
	void BeginPlay();
	void EndPlay(const EEndPlayReason::Type endPlayReason);
	void Tick(float DeltaSeconds);

public:
	AActor_PhotonVoice();

	virtual AActor_PhotonAudioIn* createAudioIn();
	virtual void destroyAudioIn(AActor_PhotonAudioIn*);
	virtual AActor_PhotonAudioOut* createAudioOut();
	virtual void destroyAudioOut(AActor_PhotonAudioOut*);

	virtual void updateState(int state, const ExitGames::Common::JString& stateStr, const ExitGames::Common::JString& joinedRoomName)
	{
		updateState(state, FString(stateStr.UTF8Representation().cstr()), FString(joinedRoomName.UTF8Representation().cstr()));
	}
	UFUNCTION(BlueprintImplementableEvent, Category = "Demo")
		void updateState(int32 state, const FString& stateStr, const FString& joinedRoomName);

	virtual void addPlayer(int playerNr, const ExitGames::Common::JString& playerName, bool local)
	{
		addPlayer(playerNr, FString(playerName.UTF8Representation().cstr()), local);
	}
	UFUNCTION(BlueprintImplementableEvent, Category = "Demo")
		void addPlayer(int32 playerNr, const FString& playerName, bool local);

	virtual void updateRoomList(const ExitGames::Common::JVector<ExitGames::Common::JString>& roomNames)
	{
		TArray<FString> a;
		for (unsigned int i = 0; i < roomNames.getSize(); ++i)
			a.Push(FString(roomNames[i].UTF8Representation().cstr()));
		updateRoomList(a);
	}
	UFUNCTION(BlueprintImplementableEvent, Category = "Demo")
		void updateRoomList(const TArray<FString>& roomNames);

	// end of Demo BaseView overrrides 

	// Exposing demo properties to blueprint 
	//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Demo)
	//	bool gridSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
		FString serverAddress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
		FString AppID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
		FString appVersion;

	// Exposing demo methods to blueprint 
	UFUNCTION(BlueprintCallable, Category = "Demo")
		void Connect(const FString& Vociename);
	UFUNCTION(BlueprintCallable, Category = "Demo")
		void Disconnect();
	UFUNCTION(BlueprintCallable, Category = "Demo")
		void ToggleEcho();

private:
	UPROPERTY(EditAnywhere, Category = "Demo")
		bool automove;
	UPROPERTY(EditAnywhere, Category = "Demo")
		bool useGroups;

	void PostEditChangeProperty(struct FPropertyChangedEvent& e);

	class SH_PhotonVoiceListener* mpPhotonLib;

	ExitGames::Voice::IAudioPusher<short>* audioInFactory();
	ExitGames::Voice::IAudioOut<short>* audioOutFactory();

public:
	//플레이어 출력
	UFUNCTION(BlueprintCallable, Category = "PhotonAudio")
		void SetMuteIn(bool bInput);
	UFUNCTION(BlueprintCallable, Category = "PhotonAudio")
		void SetMuteOut(bool bInput);
	UFUNCTION(BlueprintCallable, Category = "PhotonAudio")
		void SetMute(bool bInput);
};
