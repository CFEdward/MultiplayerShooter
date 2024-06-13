// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ShooterGameMode.generated.h"

namespace MatchState
{
    extern MULTIPLAYERSHOOTER_API const FName Cooldown;	// Match duration has been reached. Display winner and begin cooldown timer
}

class AShooterCharacter;
class AShooterPlayerState;
class AShooterPlayerController;

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	AShooterGameMode();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void PlayerEliminated(
		AShooterCharacter* ElimmedCharacter,
		AShooterPlayerController* VictimController,
		AShooterPlayerController* AttackerController
	);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

	void PlayerLeftGame(AShooterPlayerState* PlayerLeaving) const;

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime;
	UPROPERTY(EditDefaultsOnly)
	float MatchTime;
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime;
	float LevelStartingTime;

	virtual void Logout(AController* Exiting) override;

protected:

	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	
	float CountdownTime;

public:

	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
	
};
