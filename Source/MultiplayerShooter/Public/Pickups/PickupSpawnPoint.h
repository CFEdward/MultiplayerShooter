// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

class APickup;

UCLASS()
class MULTIPLAYERSHOOTER_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	
	APickupSpawnPoint();
	
	virtual void Tick(float DeltaTime) override;
	
protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<APickup>> PickupClasses;
	UPROPERTY()
	TObjectPtr<APickup> SpawnedPickup;
	
	void SpawnPickup();
	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestroyedActor);
	void SpawnPickupTimerFinished();

private:

	FTimerHandle SpawnPickupTimer;
	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMin;
	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMax;

};
