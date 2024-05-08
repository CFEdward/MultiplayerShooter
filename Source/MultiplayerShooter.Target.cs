// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MultiplayerShooterTarget : TargetRules
{
	public MultiplayerShooterTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		bUsesSteam = true;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "MultiplayerShooter" } );
	}
}
