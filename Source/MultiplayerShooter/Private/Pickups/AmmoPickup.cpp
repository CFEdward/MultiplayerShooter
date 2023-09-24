// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/AmmoPickup.h"

#include "Character/ShooterCharacter.h"
#include "ShooterComponents/CombatComponent.h"

AAmmoPickup::AAmmoPickup() :
	AmmoAmount(30),
	WeaponType(EWeaponType::EWT_MAX)
{
	
}

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (const AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor))
	{
		if (UCombatComponent* Combat = ShooterCharacter->GetCombat())
		{
			Combat->PickupAmmo(WeaponType, AmmoAmount);
		}
	}
	Destroy();
}
