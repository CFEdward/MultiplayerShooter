// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USoundCue;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class MULTIPLAYERSHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	AProjectile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;

	/**
	 * Used with server-side rewind
	 */
	bool bUseServerSideRewind;
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;

	UPROPERTY(EditAnywhere)
	float InitialSpeed;
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	void SpawnTrailSystem();
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void ExplodeDamage();

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> ImpactParticles;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> CollisionBox;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> TrailSystem;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TrailSystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	FTimerHandle DestroyTimer;
	
private:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> Tracer;

	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> TracerComponent;

	UPROPERTY(EditAnywhere)
	float DestroyTime;

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius;
	
public:	

};
