// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileBullet.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void AProjectileBullet::OnHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	// ReSharper disable once CppParameterMayBeConst
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (AController* OwnerController = OwnerCharacter->Controller)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
		}
	}
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
