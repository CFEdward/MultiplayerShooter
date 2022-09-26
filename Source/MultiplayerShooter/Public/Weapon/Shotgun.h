// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()

public:

	AShotgun();
	
	virtual void Fire(const FVector& HitTarget) override;

private:

	UPROPERTY(EditAnywhere)
	uint32 NumberOfPellets;
	
};
