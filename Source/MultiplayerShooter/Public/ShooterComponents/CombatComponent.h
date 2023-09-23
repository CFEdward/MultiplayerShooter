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
	
	void FireButtonPressed(const bool bPressed);
	
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	void JumpToShotgunEnd() const;

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();
	UFUNCTION(BlueprintCallable)
	void LaunchGrenade();
	
protected:
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void SetAiming(const bool bIsAiming);
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(const bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon() const;
	void AttachActorToRightHand(AActor* ActorToAttach) const;
	void AttachActorToLeftHand(AActor* ActorToAttach) const;
	void UpdateCarriedAmmo();
	void PlayEquippedWeaponSound() const;
	void ReloadEmptyWeapon();
	
	void DropEquippedWeapon() const;
	
	void Fire();
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);
	
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
	AShooterCharacter* Character;
	UPROPERTY()
	AShooterPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;
	bool bFireButtonPressed;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	/**
	 * HUD and crosshairs
	 */
	UPROPERTY()
	AShooterHUD* HUD;
	FHUDPackage HUDPackage;
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	
	FVector HitTarget;

	/**
	 * Aiming and FOV
	 */
	UPROPERTY(Replicated)
	bool bAiming;
	// Field of view when not aiming; set to the camera's base FOV in BeginPlay
	float DefaultFOV;
	float CurrentFOV;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed;

	// Carried ammo for the currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	UFUNCTION()
	void OnRep_CarriedAmmo();

	UPROPERTY(EditAnywhere)
	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState;
	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();

	UPROPERTY(EditAnywhere, Category = "Scope Sound")
	USoundCue* ZoomInSniperRifle;
	UPROPERTY(EditAnywhere, Category = "Scope Sound")
	USoundCue* ZoomOutSniperRifle;
	
public:
	
	FORCEINLINE void SetCombatState(const ECombatState State) { CombatState = State; }
	
};
