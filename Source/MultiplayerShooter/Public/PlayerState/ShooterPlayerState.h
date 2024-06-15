// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShooterTypes/Team.h"
#include "ShooterPlayerState.generated.h"

class AShooterCharacter;
class AShooterPlayerController;

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AShooterPlayerState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Replication notifies
	 */
	
	virtual void OnRep_Score() override;
	UFUNCTION()
	void OnRep_Defeats();
	
	void AddToScore(const float ScoreAmount);
	void AddToDefeats(const int32 DefeatsAmount);

private:

	UPROPERTY()
	TObjectPtr<AShooterCharacter> Character;
	UPROPERTY()
	TObjectPtr<AShooterPlayerController> Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team;
	UFUNCTION()
	void OnRep_Team();

public:

	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(const ETeam TeamToSet);
	
};
