// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ShooterGameMode.h"
#include "Character/ShooterCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/ShooterPlayerController.h"
#include "PlayerState/ShooterPlayerState.h"

void AShooterGameMode::PlayerEliminated(
	AShooterCharacter* ElimmedCharacter,
	AShooterPlayerController* VictimController,
	AShooterPlayerController* AttackerController)
{
	AShooterPlayerState* AttackerPlayerState =
		AttackerController ? Cast<AShooterPlayerState>(AttackerController->PlayerState) : nullptr;
	const AShooterPlayerState* VictimPlayerState =
		VictimController ? Cast<AShooterPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.0f);
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
