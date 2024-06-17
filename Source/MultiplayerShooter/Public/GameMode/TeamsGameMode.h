// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/ShooterGameMode.h"
#include "TeamsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ATeamsGameMode : public AShooterGameMode
{
	GENERATED_BODY()

public:

	ATeamsGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;
	virtual void PlayerEliminated(
			AShooterCharacter* ElimmedCharacter,
			AShooterPlayerController* VictimController,
			AShooterPlayerController* AttackerController) override;
	
	virtual void Logout(AController* Exiting) override;

protected:

	virtual void HandleMatchHasStarted() override;
};
