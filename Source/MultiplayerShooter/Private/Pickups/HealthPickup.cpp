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

void AHealthPickup::OnOverlap(AShooterCharacter* ShooterCharacter)
{
	Super::OnOverlap(ShooterCharacter);
	
	if (UBuffComponent* Buff = ShooterCharacter->GetBuff())
	{
		Buff->Heal(HealAmount, HealingTime);
	}

	Destroy();
}
