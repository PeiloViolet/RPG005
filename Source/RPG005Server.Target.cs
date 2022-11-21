// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RPG005ServerTarget : TargetRules
{
	
	
	public RPG005ServerTarget(TargetInfo Target) : base(Target) //根据你的项目名称更改此行
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("RPG005"); //根据你的项目名称更改此行
	}
}
