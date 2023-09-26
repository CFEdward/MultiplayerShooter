// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "Weapon/WeaponTypes.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

public:

	AAmmoPickup();
	
protected:
	
	virtual void OnOverlap(AShooterCharacter* ShooterCharacter) override;

private:

	UPROPERTY(EditAnywhere)
	int32 AmmoAmount;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	
};
