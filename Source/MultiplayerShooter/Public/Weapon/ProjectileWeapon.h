// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "ProjectileWeapon.generated.h"

class AProjectile;

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:

	virtual void Fire(const FVector& HitTarget) override;

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ServerSideRewindProjectileClass;
};
