// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "ShooterTypes/CombatState.h"
#include "ShooterCharacter.generated.h"

class UBoxComponent;
class UBuffComponent;
class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class AWeapon;
class UCombatComponent;
class UAnimMontage;
class AShooterPlayerController;
class USoundCue;
class AShooterPlayerState;

USTRUCT(BlueprintType)
struct FPhysAssetInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FName BoneName;
	UPROPERTY()
	float HalfHeight = 0.f;
	UPROPERTY()
	float Radius = 0.f;
	UPROPERTY()
	FTransform BoneWorldTransform;
};

UCLASS()
class MULTIPLAYERSHOOTER_API AShooterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	
	AShooterCharacter();
	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(const bool bAiming) const;
	void PlayReloadMontage();
	void ReloadMontageInterrupted(UAnimMontage* Montage, bool bInterrupted);
	void PlayElimMontage() const;
	void PlayThrowGrenadeMontage() const;
	virtual void OnRep_ReplicatedMovement() override;
	void Elim();
	void DropWeapons() const;
	virtual void Destroyed() override;
	
	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	void SpawnDefaultWeapon();

	UPROPERTY(Replicated)
	bool bDisableGameplay;
	
protected:
	
	virtual void BeginPlay() override;
	
	void RotateInPlace(const float DeltaTime);
	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void Turn(const float Value);
	void LookUp(const float Value);
	void SimProxiesTurn();
	
	virtual void Jump() override;
	
	float CalculateSpeed() const;
	void CalculateAO_Pitch();
	void AimOffset(const float DeltaTime);
	
	void PlayHitReactMontage() const;
	
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void EquipButtonPressed();
	void SwapWeaponsPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void GrenadeButtonPressed();
	
	/** Poll for any relevant classes and initialize our HUD */
	void PollInit();

	UFUNCTION()
	void ReceiveDamage(
		AActor* DamagedActor,
		float Damage,
		const UDamageType* DamageType,
		AController* InstigatorController,
		AActor* DamageCauser);

	UPROPERTY()
	TMap<FName, TObjectPtr<UBoxComponent>> HitCollisionBox;
	UPROPERTY()
	TMap<FName, TObjectPtr<UCapsuleComponent>> HitCollisionCapsule;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UCapsuleComponent>> HitCapsuleBones;
	UPROPERTY(EditAnywhere)
	TArray<FPhysAssetInformation> PhysAssetInfo;
	bool bDrawPhysAssets;
	void HitCapsuleConstruction();
	void SetupHitCapsule(FPhysAssetInformation PhysicsAssetInfo);
	
	/**
	 * Hit boxes used for server-side rewind
	 *
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Head;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Pelvis;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Spine_02;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Spine_03;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> UpperArm_L;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> UpperArm_R;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> LowerArm_L;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> LowerArm_R;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Hand_L;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Hand_R;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Backpack;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Blanket;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Thigh_L;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Thigh_R;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Calf_L;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Calf_R;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Foot_L;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Foot_R;
	*/

private:

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;
	UFUNCTION()
	void OnRep_OverlappingWeapon(const AWeapon* LastWeapon) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> Combat;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBuffComponent> Buff;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	UFUNCTION(Server, Reliable)
	void ServerSwapWeaponPressed();

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
	TObjectPtr<UAnimMontage> FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> ReloadMontage;
	bool bShouldStopReload;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> ThrowGrenadeMontage;

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
	void OnRep_Health(float LastHealth);

	/**
	 * Player Shield
	 */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield;
	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield;
	UFUNCTION()
	void OnRep_Shield(float LastShield);
	
	UPROPERTY()
	TObjectPtr<AShooterPlayerController> ShooterPlayerController;

	bool bElimmed;
	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay;
	void ElimTimerFinished();

	/**
	 * Dissolve effect
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> DissolveTimeline;
	FOnTimelineFloat DissolveTrack;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> DissolveCurve;
	UFUNCTION()
	void UpdateDissolveMaterial(const float DissolveValue);
	void StartDissolve();
	/** Dynamic instance that we can change at runtime */
	UPROPERTY(VisibleAnywhere, Category = "Elim")
	TObjectPtr<UMaterialInstanceDynamic> DynamicDissolveMaterialInstance;
	/** Material instance set on the Blueprint, used with the dynamic material instance */
	UPROPERTY(EditAnywhere, Category = "Elim")
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	/**
	 * Elim bot
	 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> ElimBotEffect;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UParticleSystemComponent> ElimBotComponent;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> ElimBotSound;

	UPROPERTY()
	TObjectPtr<AShooterPlayerState> ShooterPlayerState;

	/**
	 * Grenade
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> AttachedGrenade;

	/**
	 * Default Weapon
	 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;
	
public:

	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped() const;
	bool IsAiming() const;
	bool IsLocallyReloading() const;
	AWeapon* GetEquippedWeapon() const;
	FVector GetHitTarget() const;
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone()	const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(const float Amount) { Health = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE void SetShield(const float Amount) { Shield = Amount; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	
};
