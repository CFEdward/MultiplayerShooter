// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ShooterGameMode.h"
#include "Character/ShooterCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "GameState/ShooterGameState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/ShooterPlayerController.h"
#include "PlayerState/ShooterPlayerState.h"


namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

AShooterGameMode::AShooterGameMode() :
	WarmupTime(10.0f),
	MatchTime(120.0f),
	CooldownTime(10.0f),
	LevelStartingTime(0.0f),
	CountdownTime(0.0f)
{
	bDelayedStart = true;
}

void AShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AShooterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.0f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.0f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.0f)
		{
			// UWorld* World = GetWorld();
			// if (World)
			// {
			// 	bUseSeamlessTravel = true;
			// 	World->ServerTravel(FString("/Game/Maps/ShooterMap?listen"));
			// }
			bUseSeamlessTravel = true;
			RestartGame();
		}
	}
}

void AShooterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AShooterPlayerController* ShooterPlayer = Cast<AShooterPlayerController>(*It))
		{
			ShooterPlayer->OnMatchStateSet(MatchState);
		}
	}
}

float AShooterGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

void AShooterGameMode::PlayerEliminated(
	AShooterCharacter* ElimmedCharacter,
	AShooterPlayerController* VictimController,
	AShooterPlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	AShooterPlayerState* AttackerPlayerState = AttackerController ? Cast<AShooterPlayerState>(AttackerController->PlayerState) : nullptr;
	AShooterPlayerState* VictimPlayerState = VictimController ? Cast<AShooterPlayerState>(VictimController->PlayerState) : nullptr;
	AShooterGameState* ShooterGameState = GetGameState<AShooterGameState>();
	
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && ShooterGameState)
	{
		TArray<AShooterPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : ShooterGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}
		
		AttackerPlayerState->AddToScore(1.0f);
		ShooterGameState->UpdateTopScore(AttackerPlayerState);
		if (ShooterGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			if (AShooterCharacter* Leader = Cast<AShooterCharacter>(AttackerPlayerState->GetPawn()))
			{
				Leader->MulticastGainedTheLead();
			}
		}

		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!ShooterGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				if (AShooterCharacter* Loser = Cast<AShooterCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn()))
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AShooterPlayerController* ShooterPlayer = Cast<AShooterPlayerController>(*It); ShooterPlayer && AttackerPlayerState && VictimPlayerState)
		{
			ShooterPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}
}

void AShooterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		const int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

void AShooterGameMode::PlayerLeftGame(AShooterPlayerState* PlayerLeaving) const
{
	if (PlayerLeaving == nullptr) return;
	
	if (AShooterGameState* ShooterGameState = GetGameState<AShooterGameState>(); ShooterGameState && ShooterGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		ShooterGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	if (AShooterCharacter* CharacterLeaving = Cast<AShooterCharacter>(PlayerLeaving->GetPawn()))
	{
		CharacterLeaving->Elim(true);
	}
}

void AShooterGameMode::Logout(AController* Exiting)
{
	if (Exiting == nullptr) return;

	AShooterGameState* ShooterGameState = GetGameState<AShooterGameState>();
	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(Exiting->PlayerState);
	if (ShooterGameState && ShooterGameState->TopScoringPlayers.Contains(ShooterPlayerState))
	{
		UE_LOG(LogTemp, Warning, TEXT("Logout"));
		ShooterGameState->TopScoringPlayers.Remove(ShooterPlayerState);
	}
	
	Super::Logout(Exiting);
}
