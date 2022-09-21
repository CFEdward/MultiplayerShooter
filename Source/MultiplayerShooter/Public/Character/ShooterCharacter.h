// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "ShooterTypes/CombatState.h"
#include "ShooterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class AWeapon;
class UCombatComponent;
class UAnimMontage;
class AShooterPlayerController;
class USoundCue;
class AShooterPlayerState;

UCLASS()
class MULTIPLAYERSHOOTER_API AShooterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AShooterCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(const bool bAiming) const;
	void PlayReloadMontage() const;
	void PlayElimMontage() const;
	virtual void OnRep_ReplicatedMovement() override;
	void Elim();
	virtual void Destroyed() override;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void Turn(const float Value);
	void LookUp(const float Value);
	void SimProxiesTurn();
	virtual void Jump() override;
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void EquipButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	float CalculateSpeed() const;
	void CalculateAO_Pitch();
	void AimOffset(const float DeltaTime);
	void PlayHitReactMontage() const;
	void UpdateHUDHealth();
	// Poll for any relevant classes and initialize our HUD
	void PollInit();

	UFUNCTION()
	void ReceiveDamage(
		AActor* DamagedActor,
		float Damage,
		const UDamageType* DamageType,
		AController* InstigatorController,
		AActor* DamageCauser);

private:

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* Combat;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(const float DeltaTime);

	/**
	 * Animation montages
	 */
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;

	void HideCharacterIfCameraClose() const;

	UPROPERTY(EditAnywhere)
	float CameraThreshold;

	bool bRotateRootBone;
	float TurnThreshold;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;

	/**
	 * Player Health
	 */

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health;

	UFUNCTION()
	void OnRep_Health();

	UPROPERTY()
	AShooterPlayerController* ShooterPlayerController;

	bool bElimmed;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay;

	void ElimTimerFinished();

	/**
	 * Dissolve effect
	 */

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(const float DissolveValue);
	void StartDissolve();

	// Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = "Elim")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	// Material instance set on the Blueprint, used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category = "Elim")
	UMaterialInstance* DissolveMaterialInstance;

	/**
	 * Elim bot
	 */

	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	USoundCue* ElimBotSound;

	UPROPERTY()
	AShooterPlayerState* ShooterPlayerState;
	
public:

	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped() const;
	bool IsAiming() const;
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon() const;
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone()	const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	ECombatState GetCombatState() const;
	
};
