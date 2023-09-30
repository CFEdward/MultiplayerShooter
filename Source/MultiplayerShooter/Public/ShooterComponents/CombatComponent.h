// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/ShooterHUD.h"
#include "Weapon/WeaponTypes.h"
#include "ShooterTypes/CombatState.h"
#include "CombatComponent.generated.h"

class AProjectile;
class AShooterCharacter;
class AWeapon;
class AShooterPlayerController;
class AShooterHUD;
class USoundCue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UCombatComponent();
	
	friend AShooterCharacter;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* WeaponToEquip);
	void SwapWeapons();
	void FireButtonPressed(const bool bPressed);

	bool bLocallyReloading;
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	void JumpToShotgunEnd();

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();
	UFUNCTION(BlueprintCallable)
	void LaunchGrenade();
	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& Target);

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);
	
protected:
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void SetAiming(const bool bIsAiming);
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(const bool bIsAiming);

	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	UFUNCTION()
	void OnRep_EquippedWeapon() const;
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);
	UFUNCTION()
	void OnRep_SecondaryWeapon();
	void AttachActorToRightHand(AActor* ActorToAttach) const;
	void AttachActorToLeftHand(AActor* ActorToAttach) const;
	void AttachActorToBackpack(AActor* ActorToAttach) const;
	void UpdateCarriedAmmo();
	void PlayEquippedWeaponSound(const AWeapon* WeaponToEquip) const;
	void ReloadEmptyWeapon();
	
	void DropEquippedWeapon() const;
	
	void Fire();
	void FireProjectileWeapon();
	void FireHitScanWeapon();
	void FireShotgun();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget) const;
	void ShotgunLocalFire(const TArray<FVector_NetQuantize>& TraceHitTargets);
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(Server, Reliable)
	void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);
	
	/** Automatic fire */
	FTimerHandle FireTimer;
	bool bCanFire;
	void StartFireTimer();
	void FireTimerFinished();
	bool CanFire() const;
	/** Automatic fire */
	
	UFUNCTION(Server, Reliable)
	void ServerReload();
	void HandleReload() const;
	int32 AmountToReload();

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(const float DeltaTime);
	
	void InterpFOV(const float DeltaTime);

	void ThrowGrenade();
	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();
	void ShowAttachedGrenade(bool bShowGrenade) const;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> GrenadeClass;

private:

	UPROPERTY()
	TObjectPtr<AShooterCharacter> Character;
	UPROPERTY()
	TObjectPtr<AShooterPlayerController> Controller;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	TObjectPtr<AWeapon> SecondaryWeapon;
	
	bool bFireButtonPressed;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float BaseCrouchSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	/**
	 * HUD and crosshairs
	 */
	UPROPERTY()
	TObjectPtr<AShooterHUD> HUD;
	FHUDPackage HUDPackage;
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	
	FVector HitTarget;

	/**
	 * Aiming and FOV
	 */
	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming;
	UFUNCTION()
	void OnRep_Aiming();
	bool bAimButtonPressed;
	// Field of view when not aiming; set to the camera's base FOV in BeginPlay
	float DefaultFOV;
	float CurrentFOV;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed;

	// Carried ammo for the currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	UFUNCTION()
	void OnRep_CarriedAmmo();

	UPROPERTY(EditAnywhere)
	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState;
	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();

	UPROPERTY(EditAnywhere, Category = "Scope Sound")
	TObjectPtr<USoundCue> ZoomInSniperRifle;
	UPROPERTY(EditAnywhere, Category = "Scope Sound")
	TObjectPtr<USoundCue> ZoomOutSniperRifle;

	UPROPERTY(ReplicatedUsing = OnRep_Grenades)
	int32 Grenades = 4;
	UFUNCTION()
	void OnRep_Grenades();
	UPROPERTY(EditAnywhere)
	int32 MaxGrenades = 4;
	void UpdateHUDGrenades();
	
public:

	void SetWalkSpeeds(float BaseSpeed, float CrouchSpeed);
	FORCEINLINE void SetCombatState(const ECombatState State) { CombatState = State; }
	FORCEINLINE int32 GetGrenades() const { return Grenades; }
	bool ShouldSwapWeapons() const;
	
};
