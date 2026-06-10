// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Rakugo : ModuleRules
{
	public Rakugo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Rakugo",
			"Rakugo/Variant_Platforming",
			"Rakugo/Variant_Platforming/Animation",
			"Rakugo/Variant_Combat",
			"Rakugo/Variant_Combat/AI",
			"Rakugo/Variant_Combat/Animation",
			"Rakugo/Variant_Combat/Gameplay",
			"Rakugo/Variant_Combat/Interfaces",
			"Rakugo/Variant_Combat/UI",
			"Rakugo/Variant_SideScrolling",
			"Rakugo/Variant_SideScrolling/AI",
			"Rakugo/Variant_SideScrolling/Gameplay",
			"Rakugo/Variant_SideScrolling/Interfaces",
			"Rakugo/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
