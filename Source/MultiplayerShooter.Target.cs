// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MultiplayerShooterTarget : TargetRules
{
	public MultiplayerShooterTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		bUsesSteam = true;
		bOverrideBuildEnvironment = true;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "MultiplayerShooter" } );
	}
}
