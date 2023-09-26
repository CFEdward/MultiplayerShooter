// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "SpeedPickup.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ASpeedPickup : public APickup
{
	GENERATED_BODY()

public:

	ASpeedPickup();

protected:
	
	virtual void OnOverlap(AShooterCharacter* ShooterCharacter) override;

private:

	UPROPERTY(EditAnywhere)
	float BaseSpeedBuff;
	UPROPERTY(EditAnywhere)
	float CrouchSpeedBuff;

	UPROPERTY(EditAnywhere)
	float SpeedBuffTime;
	
};
