// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/JumpPickup.h"

#include "Character/ShooterCharacter.h"
#include "ShooterComponents/BuffComponent.h"


AJumpPickup::AJumpPickup() :
	JumpZVelocityBuff(4000.f),
	JumpBuffTime(30.f)
{
	
}

void AJumpPickup::OnOverlap(AShooterCharacter* ShooterCharacter)
{
	Super::OnOverlap(ShooterCharacter);
	
	if (UBuffComponent* Buff = ShooterCharacter->GetBuff())
	{
		Buff->BuffJump(JumpZVelocityBuff, JumpBuffTime);
	}

	Destroy();
}
