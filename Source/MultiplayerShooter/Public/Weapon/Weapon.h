// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USphereComponent;
class UWidgetComponent;
class UAnimationAsset;
class ABulletCasing;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial		UMETA(DisplayName = "Initial State"),
	EWS_Equipped	UMETA(DisplayName = "Equipped"),
	EWS_Dropped		UMETA(DisplayName = "Dropped"),

	EWS_MAX			UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class MULTIPLAYERSHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	AWeapon();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ShowPickupWidget(bool bShowWidget);

	virtual void Fire(const FVector& HitTarget);
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
private:

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABulletCasing> BulletCasingClass;
	
public:	
	
	void SetWeaponState(EWeaponState State);

	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
};
