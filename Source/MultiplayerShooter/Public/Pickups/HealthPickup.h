// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AHealthPickup : public APickup
{
	GENERATED_BODY()

public:

	AHealthPickup();
	
protected:
	
	virtual void OnOverlap(AShooterCharacter* ShooterCharacter) override;

private:

	UPROPERTY(EditAnywhere)
	float HealAmount;
	UPROPERTY(EditAnywhere)
	float HealingTime;
	
};
