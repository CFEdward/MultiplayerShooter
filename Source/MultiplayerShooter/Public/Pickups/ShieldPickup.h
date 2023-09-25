// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "ShieldPickup.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShieldPickup : public APickup
{
	GENERATED_BODY()

public:

	AShieldPickup();
	
protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

private:

	UPROPERTY(EditAnywhere)
	float ShieldReplenishAmount;
	UPROPERTY(EditAnywhere)
	float ShieldReplenishTime;
	
};
