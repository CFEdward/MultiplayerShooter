// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/CaptureTheFlagGameMode.h"

#include "CaptureTheFlag/FlagZone.h"
#include "GameState/ShooterGameState.h"
#include "Weapon/Flag.h"

void ACaptureTheFlagGameMode::PlayerEliminated(AShooterCharacter* ElimmedCharacter, AShooterPlayerController* VictimController, AShooterPlayerController* AttackerController)
{
	AShooterGameMode::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
}

void ACaptureTheFlagGameMode::FlagCaptured(const AFlag* Flag, const AFlagZone* Zone) const
{
	//bool bValidCapture = Flag->GetTeam() != Zone->Team;
	if (AShooterGameState* SGameState = Cast<AShooterGameState>(GameState))
	{
		if (Zone->Team == ETeam::ET_BlueTeam)
		{
			SGameState->BlueTeamScores();
		}
		if (Zone->Team == ETeam::ET_RedTeam)
		{
			SGameState->RedTeamScores();
		}
	}
}
