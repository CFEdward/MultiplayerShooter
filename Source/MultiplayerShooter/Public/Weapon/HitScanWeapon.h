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

	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit) const;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> ImpactParticles;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> HitSound;
	
private:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> BeamParticles;
	
};
