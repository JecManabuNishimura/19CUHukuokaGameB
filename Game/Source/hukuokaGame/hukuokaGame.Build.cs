// ファイル名：hukuokaGame.Build.cs
// 修正内容：VR用モジュールの追加
// 作成者：19CU0217 朱適

// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class hukuokaGame : ModuleRules
{
	public hukuokaGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// SomWorks :D // Add HeadMountedDisplay Modules for Use HeadMountedDisplayFunctionLibrary // 4.20, Add NavigationSystem Module
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem" });

		// SomWorks :D // Add SteamVR, SteamVRController Modules for Use Vive Components
		// SomWorks :D // 4.24 Update, Module Change // SteamVRController -> SteamVRInput
		PrivateDependencyModuleNames.AddRange(new string[] { "SteamVR", "SteamVRInput" });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
