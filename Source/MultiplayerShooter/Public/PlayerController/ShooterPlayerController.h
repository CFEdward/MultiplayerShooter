// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

class AShooterHUD;

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	void SetHUDHealth(const float Health, const float MaxHealth);
	void SetHUDScore(const float Score);
	void SetHUDDefeats(const int32 Defeats);
	void SetHUDWeaponAmmo(const int32 Ammo);
	void SetHUDCarriedAmmo(const int32 Ammo);
	virtual void OnPossess(APawn* InPawn) override;
	
protected:
	
	virtual void BeginPlay() override;

private:

	UPROPERTY()
	AShooterHUD* ShooterHUD;
	
};
