// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/ShooterGameState.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/ShooterPlayerController.h"
#include "PlayerState/ShooterPlayerState.h"


AShooterGameState::AShooterGameState() :
	RedTeamScore(0.f),
	BlueTeamScore(0.f),
	TopScore(0.0f)
{
}

void AShooterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterGameState, TopScoringPlayers);
	DOREPLIFETIME(AShooterGameState, RedTeamScore);
	DOREPLIFETIME(AShooterGameState, BlueTeamScore);
}

void AShooterGameState::UpdateTopScore(AShooterPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void AShooterGameState::OnRep_RedTeamScore()
{
	if (AShooterPlayerController* SPlayer = Cast<AShooterPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		SPlayer->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AShooterGameState::RedTeamScores()
{
	++RedTeamScore;

	if (AShooterPlayerController* SPlayer = Cast<AShooterPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		SPlayer->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AShooterGameState::OnRep_BlueTeamScore()
{
	if (AShooterPlayerController* SPlayer = Cast<AShooterPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		SPlayer->SetHUDBlueTeamScore(BlueTeamScore);
	}
}

void AShooterGameState::BlueTeamScores()
{
	++BlueTeamScore;

	if (AShooterPlayerController* SPlayer = Cast<AShooterPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		SPlayer->SetHUDBlueTeamScore(BlueTeamScore);
	}
}
