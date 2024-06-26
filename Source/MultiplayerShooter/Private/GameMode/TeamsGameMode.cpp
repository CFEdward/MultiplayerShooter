// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/TeamsGameMode.h"

#include "GameState/ShooterGameState.h"
#include "PlayerController/ShooterPlayerController.h"
#include "PlayerState/ShooterPlayerState.h"

ATeamsGameMode::ATeamsGameMode()
{
	bTeamsMatch = true;
}

void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AShooterGameState* SGameState = GetGameState<AShooterGameState>())
	{
		if (AShooterPlayerState* SPState = NewPlayer->GetPlayerState<AShooterPlayerState>(); SPState && SPState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (SGameState->BlueTeam.Num() >= SGameState->RedTeam.Num())
			{
				SGameState->RedTeam.AddUnique(SPState);
				SPState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				SGameState->BlueTeam.AddUnique(SPState);
				SPState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

float ATeamsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	const AShooterPlayerState* AttackerPState = Attacker->GetPlayerState<AShooterPlayerState>();
	const AShooterPlayerState* VictimPState = Victim->GetPlayerState<AShooterPlayerState>();
	if (AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;
	if (VictimPState == AttackerPState) return BaseDamage;
	if (AttackerPState->GetTeam() == VictimPState->GetTeam()) return 0.f;
	
	return BaseDamage;
}

void ATeamsGameMode::PlayerEliminated(AShooterCharacter* ElimmedCharacter, AShooterPlayerController* VictimController, AShooterPlayerController* AttackerController)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);

	AShooterGameState* SGameState = GetGameState<AShooterGameState>();
	const AShooterPlayerState* AttackerPlayerState = AttackerController ? Cast<AShooterPlayerState>(AttackerController->PlayerState) : nullptr;
	if (SGameState && AttackerPlayerState && VictimController != AttackerController)
	{
		if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			SGameState->BlueTeamScores();
		}
		if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			SGameState->RedTeamScores();
		}
	}
}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	if (AShooterGameState* SGameState = GetGameState<AShooterGameState>())
	{
		for (auto PState : SGameState->PlayerArray)
		{
			if (AShooterPlayerState* SPState = Cast<AShooterPlayerState>(PState.Get()); SPState && SPState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (SGameState->BlueTeam.Num() >= SGameState->RedTeam.Num())
				{
					SGameState->RedTeam.AddUnique(SPState);
					SPState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					SGameState->BlueTeam.AddUnique(SPState);
					SPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}

void ATeamsGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	AShooterGameState* SGameState = GetGameState<AShooterGameState>();
	AShooterPlayerState* SPState = Exiting->GetPlayerState<AShooterPlayerState>();
	if (SGameState && SPState)
	{
		if (SGameState->RedTeam.Contains(SPState))
		{
			SGameState->RedTeam.Remove(SPState);
		}
		if (SGameState->BlueTeam.Contains(SPState))
		{
			SGameState->BlueTeam.Remove(SPState);
		}
	}
}