// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USoundCue;
class USphereComponent;

UCLASS()
class MULTIPLAYERSHOOTER_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	

	APickup();
	
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;
	
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	float BaseTurnRate;

private:

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> OverlapSphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> PickupSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> PickupEffectComponent;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> PickupEffect;

	FTimerHandle BindOverlapTimer;
	float BindOverlapTime;
	void BindOverlapTimerFinished();

};
