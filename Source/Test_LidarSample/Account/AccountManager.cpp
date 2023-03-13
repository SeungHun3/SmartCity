// Fill out your copyright notice in the Description page of Project Settings.


#include "AccountManager.h"
#include "JSon.h"

void AAccountManager::BeginPlay()
{
	Super::BeginPlay();
	Module_Http = &FHttpModule::Get();
}

void AAccountManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

// http request
void AAccountManager::AccountPlay(FString http, const FString& verb, const FString& header)
{
	FHttpRequestRef Request = Module_Http->CreateRequest();

	/*TSharedRef<FJsonObject> RequestObj = MakeShared<FJsonObject>();
	RequestObj->SetStringField("title" , "foo");

	FString RequsetBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequsetBody);
	FJsonSerializer::Serialize(RequestObj , Writer);*/

	/*	https://
		kauth.kakao.com/oauth/authorize?client_id={app_key}&redirect_uri={redirect_uri}&response_type=code"
		https://
		kauth.kakao.com/oauth/authorize?client_id=7fc7dd006bc3e18bd670e15cfcbd9491&redirect_uri=http://localhost:8181&response_type=code" */

	/*	https://
		kauth.kakao.com/oauth/token?grant_type=authorization_code&client_id={app_key}&redirect_uri={redirect_uri}&code={authorize_code}"
		https://
		kauth.kakao.com/oauth/token?grant_type=authorization_code&client_id=7fc7dd006bc3e18bd670e15cfcbd9491&redirect_uri=https://example.com/oauth&code=request.GET.get(\"code\")" */

	/*	Google 
		https://
		identitytoolkit.googleapis.com/v1/accounts:signInWithCustomToken?key=[API_KEY] */ 

	FString CheckURL(http);
	Request->OnProcessRequestComplete().BindUObject(this, &AAccountManager::OnResponseReceived);
	Request->SetURL(CheckURL);
	Request->SetVerb(verb);

	// 헤더 설정 
	//if (!header.IsEmpty())
	//{
	//	Request->SetHeader("Authorization", header);
	//}
	
	//else
	//{
	//	// LINE
	//	if (eLogin == Eenum_Login::Line)
	//	{
	//		Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
	//	}
	//}

	UE_LOG(LogTemp, Log, TEXT("// Call Account URL :: %s "), *CheckURL);
	Request->ProcessRequest();
}
//////////////////////////////////////////////////////////////////////////////
// 로그인
void AAccountManager::AccountPlay_Kakao()
{
	//FString clientId = Kakao_REST_API_KEY;
	//// FString clientSecret = Kakao_ClientSecret;
	//// FString redirectURI("http://localhost:8181/kakaologin");
	//FString redirectURI = RedirectURI;

	//// login https://kauth.kakao.com/oauth/authorize?client_id=7fc7dd006bc3e18bd670e15cfcbd9491&redirect_uri=https://example.com/oauth&response_type=code"

	FString apiURL;
	//apiURL = "https://kauth.kakao.com/oauth/authorize?";
	//apiURL += "&client_id=" + clientId;
	//apiURL += "&redirect_uri=" + redirectURI;
	//apiURL += "&response_type=code";

	// AWS php 시스템 변경
	apiURL = "https://www.mammo6soraseado.o-r.kr/Kakao/index.php";
	// 웹 브라우저
	eLogin = Eenum_Login::Kakao;
	OpenBrowserURL.Broadcast(apiURL);
}
void AAccountManager::AccountPlay_Naver()
{
	/*FString clientId = Naver_ClientID;
	FString clientSecret = Naver_ClientSecret;
	FString redirectURI("http://localhost:8181/naverlogin");*/

	// login "https://nid.naver.com/oauth2.0/authorize?response_type=code&client_id=fweCmWGpl0e5aZD_nXnh&redirect_uri=http://localhost:8181/naverlogin&state=

	FString apiURL;
	apiURL = "https://www.mammo6soraseado.o-r.kr/Naver/index.php";
	//apiURL = "https://nid.naver.com/oauth2.0/authorize?response_type=code";
	//apiURL += "&client_id=" + clientId;
	//apiURL += "&redirect_uri=" + redirectURI;
	//// apiURL += "&state=" + ;

	eLogin = Eenum_Login::Naver;
	OpenBrowserURL.Broadcast(apiURL);

	// 접근 토근 갱신 요청
	// https://nid.naver.com/oauth2.0/token?grant_type=refresh_token&client_id={클라이언트ID}&client_secret={클라이언트Secret}&refresh_token={리플레쉬토큰}
	// AccountPlay(apiURL);
}
void AAccountManager::AccountPlay_Facebook()
{
	//FString clientId = Facebook_ClientID;
	//FString redirectURI = Facebook_RedirectURI;

	//// https://www.facebook.com/v15.0/dialog/oauth?client_id=1391639621245323&redirect_uri=https://www.mammo6soraseado.o-r.kr/Facebook/callback.php
	//// https://www.facebook.com/v15.0/dialog/oauth?client_id=1391639621245323&redirect_uri=https://www.mammo6soraseado.o-r.kr/Facebook/index.php

	FString apiURL;
	//apiURL = "https://www.facebook.com/v15.0/dialog/oauth?";
	//apiURL += "client_id=" + clientId;
	//apiURL += "&redirect_uri=" + redirectURI + "/Facebook/callback.php";
	////apiURL += "&state=123456asdf";
	//OpenBrowserURL.Broadcast(apiURL);

	eLogin = Eenum_Login::Facebook;

	// 시스템 변경
	apiURL = "https://www.mammo6soraseado.o-r.kr/Facebook/index.php";
	AccountPlay(apiURL);
	OpenBrowserURL.Broadcast(apiURL);
}
void AAccountManager::AccountPlay_Google()
{
	//FString clientId = Google_ClientID;
	//FString clientSecret = Google_ClientSecret;
	//// FString redirectURI = Google_RedirectURI; 
	//FString response_type("code"); // token ? code
	//FString scope("https://www.googleapis.com/auth/contacts.readonly");
	
	// AWS PHP 시스템 변경
	FString apiURL;
	apiURL = "https://www.mammo6soraseado.o-r.kr/Google/index.php";

	//apiURL = "https://accounts.google.com/o/oauth2/v2/auth?";
	//apiURL += "client_id=" + clientId;
	//// apiURL += "&redirect_uri=" + redirectURI;
	//apiURL += "&response_type=" + response_type;
	//apiURL += "&scope=" + scope;
	//// apiURL += "&state=";

	//// 프로필
	//// https://people.googleapis.com/v1/contactGroups?Authorization: "Bearer " + access_token

	///*	https://accounts.google.com/o/oauth2/v2/auth?
	//	scope=https://www.googleapis.com/auth/drive.metadata.readonly&
	//	include_granted_scopes=true&
	//	response_type=token&
	//	state=state_parameter_passthrough_value&
	//	redirect_uri=https%3A//oauth2.example.com/code&
	//	client_id=client_id*/

	eLogin = Eenum_Login::Google;
	OpenBrowserURL.Broadcast(apiURL);
}
void AAccountManager::AccountPlay_LINE()
{
	//FString ChannelId = LINE_ChannelID;
	//// FString clientId = LINE_ClientID;
	//FString clientSecret = LINE_ClientSecret;
	//FString redirectURI = LINE_RedirectURI;		
	//FString response_type("code"); // token ?

	// 로그인
	// 액세스 토큰 업데이트 (새로고침)
	/*
		* curl -v -X POST https://api.line.me/v2/oauth/accessToken \
		-H 'Content-Type: application/x-www-form-urlencoded' \
		--data-urlencode 'grant_type=refresh_token' \
		--data-urlencode 'client_id={channel ID}' \
		--data-urlencode 'client_secret={channel secret}' \
		--data-urlencode 'refresh_token={refresh token}'
	*/
	// Response
	/*
	* {
   "token_type":"Bearer",
   "scope":"P",
   "access_token":"bNl4YEFPI/hjFWhTqexp4MuEw...",
   "expires_in":2591977,
   "refresh_token":"8iFFRdyxNVNLWYeteMMJ"
}
	*/

	// 프로필 
	/*
		* curl -v -X GET https://api.line.me/v2/profile \
		-H 'Authorization: Bearer {access token}'
	*/
	// Response 
	/*
	* {
  "userId":"U4af4980629...",
  "displayName":"Brown",
  "pictureUrl":"https://profile.line-scdn.net/abcdefghijklmn",
  "statusMessage":"Hello, LINE!"
}
	*/

	// &state=12345abcde&scope=profile%20openid&nonce=09876xyz

	//FString state("12345abcde"); // 
	//FString scope("profile%20openid"); // profile , 20openid , 20email , openid , profile%20openid

	FString apiURL;
	//apiURL = "https://access.line.me/oauth2/v2.1/authorize?";
	//apiURL += "response_type=" + response_type;
	//apiURL += "&client_id=" + ChannelId;
	//apiURL += "&redirect_uri=" + redirectURI;
	//apiURL += "&state=" + state;
	//apiURL += "&scope=" + scope;
	// 
	// 시스템 변경 AWS php 
	apiURL = "https://www.mammo6soraseado.o-r.kr/Line/index.php";
	
	eLogin = Eenum_Login::Line;
	OpenBrowserURL.Broadcast(apiURL);
}
void AAccountManager::AccountPlay_Apple()
{
	FString apiURL;
	apiURL = "https://www.mammo6soraseado.o-r.kr/Apple/index.php";

	eLogin = Eenum_Login::Apple;
	OpenBrowserURL.Broadcast(apiURL);
}
//////////////////////////////////////////////////////////////////////////////
// rest API 유저 토큰 
void AAccountManager::getToken_kakao(const FString& code)
{
	//FString Checkcode = getURLParam(code, "code");
	//if (Checkcode.IsEmpty())
	//	return;

	//FString clientId = Kakao_REST_API_KEY;
	//// FString clientSecret = Kakao_ClientSecret;
	//// FString redirectURI("http://localhost:8181/kakaologin");
	//FString redirectURI = RedirectURI;

	////	https://kauth.kakao.com/oauth/token?grant_type=authorization_code&client_id={Kakao_REST_API_KEY}&redirect_uri={redirectURI}&code=KakaoToken

	//FString apiURL;
	//apiURL = "https://kauth.kakao.com/oauth/token?grant_type=authorization_code&";
	//apiURL += "client_id=" + clientId;
	//apiURL += "&redirect_uri=" + redirectURI;
	//apiURL += "&code=" + Checkcode;
	//// 보안 설정 추가시 필수 설정 사항
	//// apiURL += "&client_secret=" + 보안 페이지 토큰;

	//UE_LOG(LogTemp, Log, TEXT("// get Token _ Kakao "));
	//AccountPlay(apiURL);
}
void AAccountManager::getToken_Naver(const FString& code)
{
	//FString Checkcode = getURLParam(code, "code");
	//if (Checkcode.IsEmpty())
	//	return;

	//FString clientId = Naver_ClientID;
	//FString clientSecret = Naver_ClientSecret;
	//FString redirectURI("http://localhost:8181/naverlogin");

	////	 접근 토근 발급 요철
	////	https://nid.naver.com/oauth2.0/token?grant_type=authorization_code&client_id={클라이언트ID}&client_secret={클라이언트Secret}&code={코드}

	//FString apiURL;
	//apiURL = "https://nid.naver.com/oauth2.0/token?grant_type=authorization_code&";
	//apiURL += "client_id=" + clientId;
	//apiURL += "&client_secret=" + clientSecret;
	//apiURL += "&redirect_uri=" + redirectURI;
	//apiURL += "&code=" + Checkcode;
	//// apiURL += "&state=" + state;

	//UE_LOG(LogTemp, Log, TEXT("// get Token _ Naver "));
	//AccountPlay(apiURL);
}
void AAccountManager::getToken_Facebook(const FString& code)
{
	//FString Checkcode = getURLParam(code, "code");
	//if (Checkcode.IsEmpty())
	//	return;

	//FString clientId = Facebook_ClientID;
	//FString clientSecret = Facebook_ClientSecret;
	//FString redirectURI = Facebook_RedirectURI;

	////	접근 토근 발급 요철
	////	https://graph.facebook.com/v15.0/oauth/access_token?client_id=1391639621245323&redirect_uri=http://localhost:8181/facebookLogin&client_secret={app-secret}&code={code-parameter}

	////	거부된 요청 재인증
	////	https://www.facebook.com/v15.0/dialog/oauth?    client_id={app-id}    &redirect_uri={redirect-uri}    &auth_type=rerequest    scope=email

	//FString apiURL;
	//apiURL = "https://graph.facebook.com/v15.0/oauth/access_token?";
	//apiURL += "client_id=" + clientId;
	//apiURL += "&redirect_uri=" + redirectURI + "/Facebook/callback.php";
	//apiURL += "&client_secret=" + clientSecret;
	//apiURL += "&code=" + Checkcode;

	//UE_LOG(LogTemp, Log, TEXT("// get Token _ Facebook "));
	//AccountPlay(apiURL, TEXT("GET"));
}
void AAccountManager::getToken_Google(const FString& code)
{
	//FString Checkcode = getURLParam(code, "code");
	//if (Checkcode.IsEmpty())
	//	return;

	//FString clientId = Google_ClientID;
	//FString clientSecret = Google_ClientSecret;
	//// FString redirectURI = Google_RedirectURI;

	//FString apiURL;
	//apiURL = "https://graph.facebook.com/v15.0/oauth/access_token?";
	//apiURL += "client_id=" + clientId;
	//// apiURL += "&redirect_uri=" + redirectURI;
	//apiURL += "&client_secret=" + clientSecret;
	//apiURL += "&code=" + Checkcode;
	//// apiURL += "&state=" + state;

	//UE_LOG(LogTemp, Log, TEXT("// get Token _ Google "));
	//AccountPlay(apiURL);
}
void AAccountManager::getToken_LINE(const FString& code)
{
	//FString Checkcode = getURLParam(code, "code");
	//if (Checkcode.IsEmpty())
	//	return;

	//FString channelId = LINE_ChannelID;
	//// FString clientId = LINE_ClientID;
	//FString clientSecret = LINE_ClientSecret;
	//FString redirectURI = LINE_RedirectURI;

	//// 프로필 요정 
	//// 토큰 
	///*	curl -v -X POST https://api.line.me/oauth2/v2.1/token \
	//	-H 'Content-Type: application/x-www-form-urlencoded' \
	//	-d 'grant_type=authorization_code' \
	//	-d 'code=1234567890abcde' \
	//	--data-urlencode 'redirect_uri=https://example.com/auth?key=value' \
	//	-d 'client_id=1234567890' \
	//	-d 'client_secret=1234567890abcdefghij1234567890ab' \
	//	// selection -d 'code_verifier=wJKN8qz5t8SSI9lMFhBB6qwNkQBkuPZoCxzRhwLRUo1'*/

	//// Response
	///*{
	//	"access_token": "bNl4YEFPI/hjFWhTqexp4MuEw5YPs...",
	//	"expires_in": 2592000,
	//	"id_token": "eyJhbGciOiJIUzI1NiJ9...",
	//	"refresh_token": "Aa1FdeggRhTnPNNpxr8p",
	//	"scope": "profile",
	//	"token_type": "Bearer"
	//}*/

	//// 토큰 유효성 확인
	////	curl - v - X POST https ://api.line.me/v2/oauth/verify \
	//	//	-H 'Content-Type: application/x-www-form-urlencoded' \
	//	//	--data-urlencode 'access_token=bNl4YEFPI/hjFWhTqexp4MuEw5YPs7qhr6dJDXKwNPuLka...'

	//	// Response 
	//	/* {
	//	"scope":"P",
	//	"client_id":"1350031035",
	//	"expires_in":2591965
	//}	*/

	//FString apiURL;
	//apiURL = "https://api.line.me/oauth2/v2.1/token?";
	//apiURL += "grant_type=authorization_code";
	//apiURL += "&code=" + Checkcode;
	//apiURL += "&redirect_uri=" + redirectURI;
	//apiURL += "&client_id=" + channelId;
	//apiURL += "&client_secret=" + clientSecret;
	//// apiURL += "&code_verifier=";

	//UE_LOG(LogTemp, Log, TEXT("// get Token _ LINE "));
	//AccountPlay(apiURL, TEXT("POST"));
}

////////////////////////////////////////////////////////////////////////////////
// 프로필
void AAccountManager::Profile_Kakao(const FString& token)
{
	FString Kakao_token(token);
	FString CheckURL("https://kapi.kakao.com/v1/api/talk/profile");

	// https://kapi.kakao.com/v1/api/talk/profile
	// // -H "Authorization: Bearer ${ACCESS_TOKEN}"

	FString Header = "Bearer " + Kakao_token;

	UE_LOG(LogTemp, Log, TEXT("// Call Recived Profile URL :: %s "), *CheckURL);
	// 토큰 정보 
	AccountPlay(CheckURL , Header);
}
void AAccountManager::Profile_Naver(const FString& token)
{
	// 프로필 요정 
	FString Naver_token(token);
	FString CheckURL("https://openapi.naver.com/v1/nid/me");

	FString Header = "Bearer " + Naver_token;
	// CheckURL += Header;

	UE_LOG(LogTemp, Log, TEXT("// Call Recived Profile URL :: %s "), *CheckURL);

	AccountPlay(CheckURL, Header);
}
void AAccountManager::Profile_Facebook(const FString& token)
{
	// 프로필 요정 
	//  https://graph.fb.gg/v15.0/me?fields=id,name
	// curl -i -X GET \ "https://graph.fb.gg/v15.0/me?access_token={token}"

	FString facebook_token(token);
	FString CheckURL("https://graph.fb.gg/v15.0/me?");

	// FString Header = "Authorization:Bearer" + Naver_token;
	FString Header = "access_token=" + facebook_token;

	UE_LOG(LogTemp, Log, TEXT("// Call Recived Token URL _ Facebook :: %s "), *CheckURL);

	AccountPlay(CheckURL);
}
void AAccountManager::Profile_LINE(const FString& token)
{
	FString facebook_token(token);
	FString CheckURL("https://api.line.me/v2/profile");

	// FString Header = "access_token" + facebook_token;
	FString Header = "Bearer " + facebook_token;

	/*response 
	{
		"userId": "U4af4980629...",
		"displayName" : "Brown",
		"pictureUrl" : "https://profile.line-scdn.net/abcdefghijklmn",
		"statusMessage" : "Hello, LINE!"
	}*/

	UE_LOG(LogTemp, Log, TEXT("// Call Recived Token URL _ LINE :: %s "), *CheckURL);
	AccountPlay(CheckURL, Header);
}
//////////////////////////////////////////////////////////////////////////////////////////
// http 
void AAccountManager::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (Response->GetContentAsString().IsEmpty())
		return;

	TSharedPtr<FJsonObject> ResponseObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	FJsonSerializer::Deserialize(Reader, ResponseObj);

	if (ResponseObj)
	{
		FString Access_Token;
		// access_token , refresh_token , token_type , expires_in ,  에러 코드 error , 에러 메세지 error_description
		// 액세스 토큰 정보 확인
		// ResponseObj->TryGetStringField("access_token", Access_Token);

		UE_LOG(LogTemp, Log, TEXT("// Check Response Message :: %s "), *Response->GetContentAsString());

		switch (eLogin)
		{
			case Eenum_Login::Kakao:
			{
				UE_LOG(LogTemp, Log, TEXT("// Check Get Profile _ Kakao :: %s "), *Response->GetContentAsString());
				if (ResponseObj->TryGetStringField("access_token", Access_Token))
				{
					Profile_Kakao(Access_Token);
				}
				break;
			}
			case Eenum_Login::Naver:
			{
				UE_LOG(LogTemp, Log, TEXT("// Check Get Profile _ Naver :: %s "), *Response->GetContentAsString());
				if (ResponseObj->TryGetStringField("access_token", Access_Token))
				{
					Profile_Naver(Access_Token);
				}
				break;
			}
			case Eenum_Login::Google:
			{
				UE_LOG(LogTemp, Log, TEXT("// Check Get Profile _ Google :: %s "), *Response->GetContentAsString());
				if (ResponseObj->TryGetStringField("access_token", Access_Token))
				{
					UE_LOG(LogTemp, Log, TEXT("// Check Get AccessToken _ Google :: %s "), *Access_Token);
					// Profile_Google(Access_Token);
				}
				break;
			}
			case Eenum_Login::Apple:
				break;
			case Eenum_Login::Line:
			{
				UE_LOG(LogTemp, Log, TEXT("// Check Get Profile _ Line :: %s "), *Response->GetContentAsString());
				if (ResponseObj->TryGetStringField("access_token", Access_Token))
				{
					UE_LOG(LogTemp, Log, TEXT("// Check Get AccessToken _ Line :: %s "), *Access_Token);
					Profile_LINE(Access_Token);
				}
				break;
			}
			case Eenum_Login::Facebook:
			{
				UE_LOG(LogTemp, Log, TEXT("// Check Get Profile _ Facebook :: %s "), *Response->GetContentAsString());
				if (ResponseObj->TryGetStringField("access_token", Access_Token))
				{
					UE_LOG(LogTemp, Log, TEXT("// Check Get AccessToken _ Facebook :: %s "), *Access_Token);
					Profile_Facebook(Access_Token);
				}
				break;
			}
		}
		return;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("// OnResponseReceived failed URL :: %s "), *Response->GetContentAsString());
		FString CheckCallbackURL = Response->GetContentAsString();
		AccountPlay(CheckCallbackURL);
		OpenBrowserURL.Broadcast(CheckCallbackURL);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("// Error ResponseObj NULL failed :: %s "), *Response->GetContentAsString());
}

// URL 문자열 탐색, code , state 
FString AAccountManager::getURLParam(const FString& URL, const FString& URLparam)
{
	FString Param = URL;
	
	FString Parse = URLparam + "=";

	int findindex = Param.Find(Parse, ESearchCase::IgnoreCase);
	if (findindex < 0)
		return FString();

	int strLen = Parse.Len();
	int parsetIndex = findindex + strLen;

	Param.RemoveAt(0, parsetIndex);

	int findstr = Param.Find("&");
	int findParseindex = Param.Len() - findstr;

	if (findstr != -1)
		Param.RemoveAt(findstr, findParseindex);

	UE_LOG(LogTemp, Log, TEXT("// getURLParam ( %s ) / %s : %s"), *URL, *Parse, *Param);
	return Param;
}

// 웹 브라우저 URL 탐지 & 토큰 반환 
void AAccountManager::OnUrlChanged_getToken(const FString& code)
{
	switch (eLogin)
	{
	case Eenum_Login::Kakao:
		getToken_kakao(code);
		break;
	case Eenum_Login::Naver:
		getToken_Naver(code);
		break;
	case Eenum_Login::Google:
		getToken_Google(code);
		break;
	case Eenum_Login::Apple:
		// getToken_Apple(code);
		break;
	case Eenum_Login::Line:
		getToken_LINE(code);
		break;
	case Eenum_Login::Facebook:
		getToken_Facebook(code);
		break;
	}
}