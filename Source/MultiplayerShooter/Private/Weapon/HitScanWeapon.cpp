// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/HitScanWeapon.h"
#include "Character/ShooterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PlayerController/ShooterPlayerController.h"
#include "ShooterComponents/LagCompensationComponent.h"
#include "Sound/SoundCue.h"


AHitScanWeapon::AHitScanWeapon()
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
			ShooterCharacter && InstigatorController)
		{
			if (const bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled(); HasAuthority() && bCauseAuthDamage)
			{
				UGameplayStatics::ApplyDamage(
					ShooterCharacter,
					Damage,
					InstigatorController,
					this, 
					UDamageType::StaticClass()
				);
			}
			if (!HasAuthority() && bUseServerSideRewind)
			{
				ShooterOwnerCharacter = ShooterOwnerCharacter == nullptr ? Cast<AShooterCharacter>(OwnerPawn) : ShooterOwnerCharacter.Get();
				ShooterOwnerController = ShooterOwnerController == nullptr ? Cast<AShooterPlayerController>(InstigatorController) : ShooterOwnerController.Get();
				if (ShooterOwnerController && ShooterOwnerCharacter && ShooterOwnerCharacter->GetLagCompensation() && ShooterOwnerCharacter->IsLocallyControlled())
				{
					ShooterOwnerCharacter->GetLagCompensation()->ServerScoreRequest(
						ShooterCharacter,
						Start,
						HitTarget,
						ShooterOwnerController->GetServerTime() - ShooterOwnerController->SingleTripTime,
						this);
				}
			}
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
		const FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;
		
		World->LineTraceSingleByChannel(OutHit, TraceStart, End, ECollisionChannel::ECC_Visibility);
		if (OutHit.bBlockingHit)
		{
			const FVector BeamEnd = OutHit.ImpactPoint;
			DrawDebugSphere(World, BeamEnd, 16.f, 12, FColor::Orange, false, 8.f);
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
