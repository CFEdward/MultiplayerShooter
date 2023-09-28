// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/WeaponSpawnPoint.h"
#include "Weapon/Weapon.h"


AWeaponSpawnPoint::AWeaponSpawnPoint() :
	SpawnWeaponTimeMin(0.f),
	SpawnWeaponTimeMax(0.f)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AWeaponSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	StartSpawnWeaponTimer();
}

void AWeaponSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AWeaponSpawnPoint::SpawnWeapon()
{
	if (const int32 NumWeaponClasses = WeaponClasses.Num(); NumWeaponClasses > 0)
	{
		const int32 Selection = FMath::RandRange(0, NumWeaponClasses - 1);
		SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClasses[Selection], GetActorTransform());

		if (HasAuthority() && SpawnedWeapon)
		{
			GetWorldTimerManager().ClearTimer(SpawnWeaponTimer);
			SpawnedWeapon->OnEquip.AddDynamic(this, &ThisClass::StartSpawnWeaponTimer);
		}
	}
}

void AWeaponSpawnPoint::StartSpawnWeaponTimer()
{
	const float SpawnTime = FMath::FRandRange(SpawnWeaponTimeMin, SpawnWeaponTimeMax);
	GetWorldTimerManager().SetTimer(SpawnWeaponTimer, this, &ThisClass::SpawnWeaponTimerFinished, SpawnTime);
	UE_LOG(LogTemp, Warning, TEXT("Spawn timer set for %s"), *GetName());
}

void AWeaponSpawnPoint::SpawnWeaponTimerFinished()
{
	if (HasAuthority())
	{
		SpawnWeapon();
	}
}
