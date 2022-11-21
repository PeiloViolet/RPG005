// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RPG005 : ModuleRules
{
	public RPG005(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore" ,
			"AIModule" ,
			"GameplayTags",
			"AdvancedRPGSystem",
			"AscentCoreInterfaces",
			"InventorySystem",
			"CollisionsManager",
			"AnimGraphRuntime"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{ 
			"NetCore", "PhysicsCore", "GameplayTags", "AnimGraphRuntime", "ControlRig", "RigVM", "Niagara",
			/*"AnimGraph", "BlueprintGraph",*/
			"AIModule", "EnhancedInput", 
			"Slate",
			"SlateCore",
			"GameplayTags",
			"OnlineSubsystem", "OnlineSubsystemUtils", "UMG",
			"Slate", "SlateCore","AIModule",
			"TargetingSystem", "CinematicCameraManager","NavigationSystem",
			"PhysicsCore", "DeveloperSettings", "AscentSaveSystem", "AscentCombatFramework", "MountSystem","AnimGraphRuntime",
			"AdvancedRPGSystem",  "AscentCoreInterfaces", "CinematicCameraManager", "CollisionsManager", "TargetingSystem",
			"AbilitySystem", "CraftingSystem", "CombinedAnimationsSystem", "ExecutionsSystem", "InventorySystem", "MusicManager",
			"SpellActions", "AscentSaveSystem"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
