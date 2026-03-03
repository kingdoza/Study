// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Study : ModuleRules
{
	public Study(ReadOnlyTargetRules Target) : base(Target)
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
			"Study",
			"Study/Variant_Platforming",
			"Study/Variant_Platforming/Animation",
			"Study/Variant_Combat",
			"Study/Variant_Combat/AI",
			"Study/Variant_Combat/Animation",
			"Study/Variant_Combat/Gameplay",
			"Study/Variant_Combat/Interfaces",
			"Study/Variant_Combat/UI",
			"Study/Variant_SideScrolling",
			"Study/Variant_SideScrolling/AI",
			"Study/Variant_SideScrolling/Gameplay",
			"Study/Variant_SideScrolling/Interfaces",
			"Study/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
