// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/ShooterHUD.h"
#include "CombatComponent.generated.h"

class AShooterCharacter;
class AWeapon;
class AShooterPlayerController;
class AShooterHUD;

#define TRACE_LENGTH 80'000.0f

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

protected:
	
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	void Fire();

	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	void InterpFOV(float DeltaTime);

	/**
	 * Automatic fire
	 */
	
	FTimerHandle FireTimer;
	
	bool bCanFire;
	
	void StartFireTimer();
	void FireTimerFinished();

private:

	UPROPERTY()
	AShooterCharacter* Character;
	UPROPERTY()
	AShooterPlayerController* Controller;
	UPROPERTY()
	AShooterHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	/**
	 * HUD and crosshairs
	 */
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	
	FVector HitTarget;
	
	FHUDPackage HUDPackage;

	/**
	 * Aiming and FOV
	 */
	// Field of view when not aiming; set to the camera's base FOV in BeginPlay
	float DefaultFOV;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed;
	
public:
	

		
};
