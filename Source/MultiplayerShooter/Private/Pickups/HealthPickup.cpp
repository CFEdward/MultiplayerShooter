// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/HealthPickup.h"
#include "Character/ShooterCharacter.h"
#include "ShooterComponents/BuffComponent.h"


AHealthPickup::AHealthPickup() :
	HealAmount(100.f),
	HealingTime(5.f)
{
	bReplicates = true;
}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (const AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor))
	{
		if (UBuffComponent* Buff = ShooterCharacter->GetBuff())
		{
			Buff->Heal(HealAmount, HealingTime);
		}
	}

	Destroy();
}
