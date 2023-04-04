// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;

public class Test_LidarSample : ModuleRules
{
	private string ModulePath
	{
		get { return ModuleDirectory; }
	}

	// Photon lib 
	private string PhotonPath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "..", "Photon")); }
	}

	// gpg lib // \gpg\lib\c++\armeabi-v7a\libgpg.a
	private string GpgPath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "..", "gpg")); }
	}

	// firebase sdk lib
	private string FirebasePath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "..", "firebase_cpp_sdk")); }
	}

	private delegate void AddLibFunction(ReadOnlyTargetRules target, string name);
	private readonly string[] libraries = { "Common", "Photon", "LoadBalancing", "Chat", "PhotonVoice" }; // "Chat", "PhotonVoice", "crypto", "websockets", "ssl"
	private readonly string[] Firebaselibraries = { "firebase_app", "firebase_auth", "firebase_functions" }; // , "firebase_admob", "firebase_analytics",  "firebase_database", "firebase_dynamic_links", "firebase_firestore", "firebase_gma", "firebase_installations", "firebase_messaging", "firebase_remote_config", "firebase_storage"
	private readonly string libArm = "armeabi-v7a_libc++.a";

	// armeabi-v7a
	// armeabi-v7a_libc++			// 4.27
	// armeabi-v7a_no-rtti 
	// armeabi-v7a_libc++_no-rtti

	// Photon voice 
	// PhotonVoice-cpp_vc16_release_windows_md_Win32.lib
	// PhotonVoice-cpp_vc16_release_windows_md_x64.lib

	private void AddPhotonLibPathWin(ReadOnlyTargetRules target, string name)
	{
		string PlatformString = (target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "Win32";
		PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Windows",
			name + "-cpp_vc16_release_windows_md_" + PlatformString + ".lib"));
	}

	private void AddPhotonLibPathAndroid(ReadOnlyTargetRules target, string name)
	{
		PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Android",
			"lib" + name.ToLower() + "-cpp-static_release_android_" + libArm));
	}

	public void LoadPhotonSDK(ReadOnlyTargetRules target)
	{
		// Include path
		PublicIncludePaths.Add(PhotonPath);
		// PublicIncludePaths.Add(GpgPath);
		
		AddLibFunction AddLibFunc;

		if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
		{
			PublicDefinitions.Add("_EG_WINDOWS_PLATFORM");
			PublicDefinitions.Add("INTERNAL_EXPERIMENTAL");
			
			AddLibFunc = AddPhotonLibPathWin;

			// // Firebase Test cs
			//for (int i = 0; i < Firebaselibraries.Length; i++)
			//{                                                                                       // MD MT       // Debug Release
			//	PublicAdditionalLibraries.Add(Path.Combine(FirebasePath, "libs", "windows", "VS2019", "MD", "x64", "Release", Firebaselibraries[i] + ".lib"));
			//}
			
			PublicIncludePaths.Add(FirebasePath + "/include");
			// PrivateIncludePaths.Add(FirebasePath + "/include");

			// 보이스챗 추가 라이브러리
			string PlatformString = (target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "Win32";
			PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Windows", "opus_egpv_vc16_release_windows_md_" + PlatformString + ".lib"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Android)
		{
			//Target.AndroidPlatform.Architectures
			PublicDefinitions.Add("_EG_ANDROID_PLATFORM");
			AddLibFunc = AddPhotonLibPathAndroid;

			// 포톤 라이브러리 구성 변경에 따른 추가 사항
			PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Android", "lib" + "crypto" + "_release_android_" + libArm));
			PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Android", "lib" + "websockets" + "_release_android_" + libArm));
			PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Android", "lib" + "ssl" + "_release_android_" + libArm));

			PrivateDependencyModuleNames.Add("Launch");

			// gpg
			//PublicAdditionalLibraries.Add(Path.Combine(GpgPath, "lib", "c++", "armeabi-v7a", "libgpg.a"));
			//PrivateDependencyModuleNames.Add("OnlineSubsystemGooglePlay");
			//PrivateDependencyModuleNames.Add("AndroidAdvertising");

		}
		else
		{
			throw new Exception("\nTarget platform not supported: " + Target.Platform);
		}

		for (int i = 0; i < libraries.Length; i++)
		{
			AddLibFunc(target, libraries[i]);
		}
	}

	public Test_LidarSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Voice" ,
			"Networking", "Sockets","OnlineSubsystem", "OnlineSubsystemUtils",
			"LibOVRPlatform", 
			"Slate", "SlateCore", "WebBrowser", "HTTP",
			"PlayFab", "PlayFabCpp", "PlayFabCommon",
			// "Firebase",
			// "Facebook", 
			"Json" ,
			"JsonUtilities",
			// 외부 플러그인
			"UROSBridge",
			// "FirebaseAuthentication",
			 });

		PrivateDependencyModuleNames.AddRange(new string[] { "UMG", "WebSockets",
			"OnlineSubsystem" ,
			"OnlineSubsystemUtils",
			"NavigationSystem", "AIModule" ,
		});

		//Load Photon Cloud
		LoadPhotonSDK(Target);
	}
}
