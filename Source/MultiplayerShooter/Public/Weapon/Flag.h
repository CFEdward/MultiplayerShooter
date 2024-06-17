// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "Flag.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AFlag : public AWeapon
{
	GENERATED_BODY()

public:

	AFlag();

	virtual void Dropped() override;

	void ResetFlag();

protected:

	virtual void BeginPlay() override;
	
	virtual void OnEquipped() override;
	virtual void OnDropped() override;

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> FlagPoleMesh;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FlagMesh;

	FTransform InitialTransform;

public:

	FORCEINLINE UStaticMeshComponent* GetFlagPoleMesh() const { return FlagPoleMesh; }
	FORCEINLINE FTransform GetInitialTransform() const { return InitialTransform; }
};
