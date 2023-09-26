// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/AmmoPickup.h"
#include "Character/ShooterCharacter.h"
#include "ShooterComponents/CombatComponent.h"


AAmmoPickup::AAmmoPickup() :
	AmmoAmount(30),
	WeaponType(EWeaponType::EWT_MAX)
{
	
}

void AAmmoPickup::OnOverlap(AShooterCharacter* ShooterCharacter)
{
	Super::OnOverlap(ShooterCharacter);
	
	if (UCombatComponent* Combat = ShooterCharacter->GetCombat())
	{
		Combat->PickupAmmo(WeaponType, AmmoAmount);
	}
	
	Destroy();
}
