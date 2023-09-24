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

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

private:

	UPROPERTY(EditAnywhere)
	float BaseSpeedBuff;
	UPROPERTY(EditAnywhere)
	float CrouchSpeedBuff;

	UPROPERTY(EditAnywhere)
	float SpeedBuffTime;
	
};
