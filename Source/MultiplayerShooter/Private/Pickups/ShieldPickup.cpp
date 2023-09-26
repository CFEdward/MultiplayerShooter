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

void AShieldPickup::OnOverlap(AShooterCharacter* ShooterCharacter)
{
	Super::OnOverlap(ShooterCharacter);
	
	if (UBuffComponent* Buff = ShooterCharacter->GetBuff())
	{
		Buff->ReplenishShield(ShieldReplenishAmount, ShieldReplenishTime);
	}

	Destroy();
}
