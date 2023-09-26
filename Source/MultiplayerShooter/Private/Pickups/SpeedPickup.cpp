// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/SpeedPickup.h"
#include "Character/ShooterCharacter.h"
#include "ShooterComponents/BuffComponent.h"


ASpeedPickup::ASpeedPickup() :
	BaseSpeedBuff(1600.f),
	CrouchSpeedBuff(850.f),
	SpeedBuffTime(30.f)
{
	
}

void ASpeedPickup::OnOverlap(AShooterCharacter* ShooterCharacter)
{
	Super::OnOverlap(ShooterCharacter);
	
	if (UBuffComponent* Buff = ShooterCharacter->GetBuff())
	{
		Buff->BuffSpeed(BaseSpeedBuff, CrouchSpeedBuff, SpeedBuffTime);
	}

	Destroy();
}
