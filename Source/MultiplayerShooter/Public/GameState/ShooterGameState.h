// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ShooterGameState.generated.h"

class AShooterPlayerState;

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterGameState : public AGameState
{
	GENERATED_BODY()

public:

	AShooterGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(AShooterPlayerState* ScoringPlayer);
	
	UPROPERTY(Replicated)
	TArray<AShooterPlayerState*> TopScoringPlayers;

private:

	float TopScore;
	
};
