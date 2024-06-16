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

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> FlagPoleMesh;
private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> FlagMesh;
};
