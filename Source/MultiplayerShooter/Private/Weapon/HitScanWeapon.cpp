// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/HitScanWeapon.h"

#include "Character/ShooterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Weapon/WeaponTypes.h"

AHitScanWeapon::AHitScanWeapon() :
	Damage(20.0f),
	DistanceToSphere(800.0f),
	SphereRadius(75.0f),
	bUseScatter(false)
{
	
}

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	if (const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash"))
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		
		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);

		if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(FireHit.GetActor());
			ShooterCharacter && HasAuthority() && InstigatorController)
		{
			UGameplayStatics::ApplyDamage(
				ShooterCharacter,
				Damage,
				InstigatorController,
				this, 
				UDamageType::StaticClass()
			);
		}
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactParticles,
				FireHit.ImpactPoint,
				FireHit.ImpactNormal.Rotation()
			);
		}
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint);
		}
	}
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit) const
{
	if (const UWorld* World = GetWorld())
	{
		const FVector End =
			bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;
		World->LineTraceSingleByChannel(OutHit, TraceStart, End, ECollisionChannel::ECC_Visibility);
		if (OutHit.bBlockingHit)
		{
			const FVector BeamEnd = OutHit.ImpactPoint;
			if (BeamParticles)
			{
				if (UParticleSystemComponent* Beam =
					UGameplayStatics::SpawnEmitterAtLocation(
						World,
						BeamParticles,
						TraceStart,
						FRotator::ZeroRotator,
						true))
				{
					Beam->SetVectorParameter(FName("Target"), BeamEnd);
				}
			}
		}
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget) const
{
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.0f, SphereRadius);
	const FVector EndLoc = SphereCenter + RandVec;
	const FVector ToEndLoc = EndLoc - TraceStart;

	// DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	// DrawDebugSphere(GetWorld(), EndLoc, 4.0f, 12, FColor::Orange, true);
	// DrawDebugLine(
	// 	GetWorld(),
	// 	TraceStart,
	// 	FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
	// 	FColor::Cyan, 
	// 	true
	// );
	
	return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}
