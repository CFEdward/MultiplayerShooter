// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "JumpPickup.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AJumpPickup : public APickup
{
	GENERATED_BODY()

public:

	AJumpPickup();
	
protected:
	
	virtual void OnOverlap(AShooterCharacter* ShooterCharacter) override;

private:

	UPROPERTY(EditAnywhere)
	float JumpZVelocityBuff;
	UPROPERTY(EditAnywhere)
	float JumpBuffTime;
	
};
