// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ShooterComponents/CombatComponent.h"
#include "Weapon/Weapon.h"
#include "ShooterTypes/CombatState.h"


void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter == nullptr) return;

	FVector Velocity = ShooterCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	Speed = Velocity.Size();

	bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f;
	bWeaponEquipped = ShooterCharacter->IsWeaponEquipped();
	EquippedWeapon = ShooterCharacter->GetEquippedWeapon();
	bIsCrouched = ShooterCharacter->bIsCrouched;
	bAiming = ShooterCharacter->IsAiming();
	TurningInPlace = ShooterCharacter->GetTurningInPlace();
	bRotateRootBone = ShooterCharacter->ShouldRotateRootBone();
	bElimmed = ShooterCharacter->IsElimmed();
	bHoldingTheFlag = ShooterCharacter->IsHoldingTheFlag();

	// Offset Yaw for Strafing
	const FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
	const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
	const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.0f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.0f);
	Lean = FMath::Clamp(Interp, -90.0f, 90.0f);

	AO_Yaw = ShooterCharacter->GetAO_Yaw();
	AO_Pitch = ShooterCharacter->GetAO_Pitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && ShooterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		ShooterCharacter->GetMesh()->TransformToBoneSpace(
			FName("hand_r"),
			LeftHandTransform.GetLocation(),
			FRotator::ZeroRotator,
			OutPosition,
			OutRotation
		);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (ShooterCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			const FTransform RightHandTransform = ShooterCharacter->GetMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World);
			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
				RightHandTransform.GetLocation(),
				RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - ShooterCharacter->GetHitTarget())
				);
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.0f);
		}

		/** Debug lines
		FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(
			FName("MuzzleFlash"),
			ERelativeTransformSpace::RTS_World);
		FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		DrawDebugLine(
			GetWorld(),
			MuzzleTipTransform.GetLocation(),
			MuzzleTipTransform.GetLocation() + MuzzleX * 1000.0f,
			FColor::Red);
		DrawDebugLine(
			GetWorld(),
			MuzzleTipTransform.GetLocation(),
			ShooterCharacter->GetHitTarget(),
			FColor::Orange);
		*/
	}

	bUseFABRIK = ShooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
	if (ShooterCharacter->IsLocallyControlled() && ShooterCharacter->GetCombatState() != ECombatState::ECS_ThrowingGrenade && ShooterCharacter->bFinishedSwapping)
	{
		bUseFABRIK = !ShooterCharacter->IsLocallyReloading();
	}
	bUseAimOffsets = ShooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !ShooterCharacter->GetDisableGameplay();
	bTransformRightHand = ShooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !ShooterCharacter->GetDisableGameplay();
}

void UShooterAnimInstance::OnReloadFailedToBlendOut(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if (bInterrupted)
	{
		ShooterCharacter->GetCombat()->FinishReloading();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Animation is blending out"));
	}
}

void UShooterAnimInstance::OnReloadSucceedAnimationEnd(UAnimMontage* AnimMontage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("Animation has completed"));
}
