using UnrealBuildTool;


public class RPG005Editor : ModuleRules
{
	public RPG005Editor(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { });
		PrivatePCHHeaderFile = "ALSEditor/Nodes/AlsAnimGraphNode_CurvesBlend.h";
		//OverridePackageType = PackageOverrideType.GameUncookedOnly;
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]{"Core", "CoreUObject","Engine", "InputCore", "RPG005", 
			"AnimGraph", "BlueprintGraph", "AnimationBlueprintLibrary", "AnimationModifiers"});
	}
}