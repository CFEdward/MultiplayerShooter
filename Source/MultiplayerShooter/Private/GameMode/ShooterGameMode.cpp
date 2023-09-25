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

void AShooterGameMode::PlayerEliminated(
	AShooterCharacter* ElimmedCharacter,
	AShooterPlayerController* VictimController,
	AShooterPlayerController* AttackerController)
{
	AShooterPlayerState* AttackerPlayerState =
		AttackerController ? Cast<AShooterPlayerState>(AttackerController->PlayerState) : nullptr;
	AShooterPlayerState* VictimPlayerState =
		VictimController ? Cast<AShooterPlayerState>(VictimController->PlayerState) : nullptr;
	AShooterGameState* ShooterGameState = GetGameState<AShooterGameState>();
	
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && ShooterGameState)
	{
		AttackerPlayerState->AddToScore(1.0f);
		ShooterGameState->UpdateTopScore(AttackerPlayerState);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
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
