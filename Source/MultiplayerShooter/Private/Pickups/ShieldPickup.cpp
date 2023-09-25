// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/ShieldPickup.h"
#include "Character/ShooterCharacter.h"
#include "ShooterComponents/BuffComponent.h"


AShieldPickup::AShieldPickup() :
	ShieldReplenishAmount(100.f),
	ShieldReplenishTime(5.f)
{
	bReplicates = true;
}

void AShieldPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (const AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor))
	{
		if (UBuffComponent* Buff = ShooterCharacter->GetBuff())
		{
			Buff->ReplenishShield(ShieldReplenishAmount, ShieldReplenishTime);
		}
	}

	Destroy();
}