// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Http.h>
#include "AccountManager.generated.h"

UENUM(BlueprintType)
enum class Eenum_Login : uint8
{
	Kakao = 0, // UMETA(DIsplayName = "KakaoName")
	Naver,
	Google,
	Apple,
	Line,
	Facebook,
	Max
};

UENUM(BlueprintType)
enum class EAuthority : uint8
{
	Account = 0,
	Code,
	Token,
	Authority,

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOpenBrowserURL, FString, token);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKakaoTokenURL, FString, token);

UCLASS()
class TEST_LIDARSAMPLE_API AAccountManager : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
		FKakaoTokenURL KakaoTokenURL;
	UPROPERTY(BlueprintAssignable)
		FOpenBrowserURL OpenBrowserURL;

	UPROPERTY(BlueprintReadWrite)
		Eenum_Login eLogin = Eenum_Login::Max;

private:

	FHttpModule* Module_Http;

	EAuthority Authority_Level = EAuthority::Account;

	bool bToken_code = false;

public:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 문자 탐색
	UFUNCTION(BlueprintCallable)
		FString getURLParam(const FString& URL, const FString& URLparam);

	// 문자 탐색
	UFUNCTION(BlueprintCallable)
		void OnUrlChanged_getToken(const FString& code);

	// 로그인
	UFUNCTION(BlueprintCallable)
		void AccountPlay(FString http, const FString& verb = "GET", const FString& header = "");

	// 로그인 페이지
	UFUNCTION(BlueprintCallable)
		void AccountPlay_Kakao();
	UFUNCTION(BlueprintCallable)
		void AccountPlay_Naver();
	UFUNCTION(BlueprintCallable)
		void AccountPlay_Facebook();
	UFUNCTION(BlueprintCallable)
		void AccountPlay_Google();
	UFUNCTION(BlueprintCallable)
		void AccountPlay_LINE();
	UFUNCTION(BlueprintCallable)
		void AccountPlay_Apple();

private:

	// 프로필
	void Profile_Kakao(const FString& token);
	void Profile_Naver(const FString& token);
	void Profile_Facebook(const FString& token);
	void Profile_LINE(const FString& token);
		
	// 토큰
	void getToken_kakao(const FString& code);
	void getToken_Naver(const FString& code);
	void getToken_Facebook(const FString& code);
	void getToken_Google(const FString& code);
	void getToken_LINE(const FString& code);

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	
};
