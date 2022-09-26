// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:

	AHitScanWeapon();
	
	virtual void Fire(const FVector& HitTarget) override;

protected:

	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);

private:

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;

	/**
	 * Trace end with scatter
	 */

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter;
	
};
