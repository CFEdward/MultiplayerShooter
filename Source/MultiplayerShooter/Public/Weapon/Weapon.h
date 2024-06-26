// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "ShooterTypes/Team.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquip);

class USphereComponent;
class UWidgetComponent;
class UAnimationAsset;
class ABulletCasing;
class UTexture2D;
class AShooterCharacter;
class AShooterPlayerController;
class USoundCue;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial				UMETA(DisplayName = "Initial State"),
	EWS_Equipped			UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary	UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped				UMETA(DisplayName = "Dropped"),

	EWS_MAX					UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_HitScan		UMETA(DisplayName = "Hit Scan Weapon"),
	EFT_Projectile	UMETA(DisplayName = "Projectile Weapon"),
	EFT_Shotgun		UMETA(DisplayName = "Shotgun Weapon"),

	EFT_MAX			UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class MULTIPLAYERSHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	
	AWeapon();
	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	void ShowPickupWidget(const bool bShowWidget) const;
	virtual void Fire(const FVector& HitTarget);
	virtual void Dropped();
	void AddAmmo(const int32 AmmoToAdd);
	
	/**
	 * Textures for the weapon crosshairs
	 */
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	TObjectPtr<UTexture2D> CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	TObjectPtr<UTexture2D> CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	TObjectPtr<UTexture2D> CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	TObjectPtr<UTexture2D> CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	TObjectPtr<UTexture2D> CrosshairsBottom;

	/**
	 * Automatic fire
	 */
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireDelay;
	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bAutomatic;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> EquipSound;

	/**
	 * Enable or disable custom depth
	 */
	void EnableCustomDepth(bool bEnable) const;

	UPROPERTY(BlueprintAssignable)
	FOnEquip OnEquip;

	UPROPERTY(EditAnywhere)
	EFireType FireType;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter;
	FVector TraceEndWithScatter(const FVector& HitTarget) const;

	UPROPERTY(EditAnywhere)
	float DropWeaponImpulse;
	
protected:
	
	virtual void BeginPlay() override;

	virtual void OnWeaponStateSet();
	virtual void OnEquipped();
	virtual void OnEquippedSecondary();
	virtual void OnDropped();

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	UPROPERTY()
	TObjectPtr<AShooterCharacter> ShooterOwnerCharacter;
	UPROPERTY()
	TObjectPtr<AShooterPlayerController> ShooterOwnerController;
	
	/**
	 * Trace end with scatter
	 */
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere;
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius;

	UPROPERTY(EditAnywhere)
	float Damage;
	UPROPERTY(EditAnywhere)
	float HeadShotDamage;

	UPROPERTY(Replicated, EditAnywhere)
	bool bUseServerSideRewind;
	UFUNCTION()
	void OnPingTooHigh(const bool bPingTooHigh);
	
private:

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USphereComponent> AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;
	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<UWidgetComponent> PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<UAnimationAsset> FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABulletCasing> BulletCasingClass;

	/**
	 * Zoomed FOV while aiming
	 */
	UPROPERTY(EditAnywhere)
	float ZoomedFOV;
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed;

	UPROPERTY(EditAnywhere)
	int32 Ammo;
	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);
	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);
	void SpendRound();
	
	/**
	 * The number of unprocessed server requests for Ammo
	 * Incremented in SpendRound, decremented in ClientUpdateAmmo
	 */
	int32 Sequence;

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	FTimerHandle DestroyDroppedWeaponTimerHandle;
	UPROPERTY(EditAnywhere)
	float DestroyDroppedWeaponTime;
	void DestroyDroppedWeapon();

	UPROPERTY(EditAnywhere)
	float BaseTurnRate;

	UPROPERTY(EditAnywhere)
	ETeam Team;
	
public:	
	
	void SetWeaponState(const EWeaponState State);
	bool IsEmpty() const;
	bool IsFull() const;
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }
	FORCEINLINE ETeam GetTeam() const { return Team; }
};
