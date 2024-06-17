// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/TeamsGameMode.h"
#include "CaptureTheFlagGameMode.generated.h"

class AFlagZone;
class AFlag;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ACaptureTheFlagGameMode : public ATeamsGameMode
{
	GENERATED_BODY()

public:
	
	virtual void PlayerEliminated(
				AShooterCharacter* ElimmedCharacter,
				AShooterPlayerController* VictimController,
				AShooterPlayerController* AttackerController) override;

	void FlagCaptured(const AFlag* Flag, const AFlagZone* Zone) const;
};
