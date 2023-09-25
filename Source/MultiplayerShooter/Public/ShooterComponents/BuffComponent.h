// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


class UNiagaraSystem;
class AShooterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UBuffComponent();

	friend AShooterCharacter;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Heal(float HealAmount, float HealingTime);
	void ReplenishShield(float ShieldAmount, float ReplenishTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void BuffJump(float BuffJumpVelocity, float BuffTime);
	
protected:
	
	virtual void BeginPlay() override;

	void HealRampUp(float DeltaTime);

	void ShieldRampUp(float DeltaTime);

private:

	UPROPERTY()
	TObjectPtr<AShooterCharacter> Character;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnCharacterEffect(UNiagaraSystem* Effect);
	
	/**
	 *  Heal buff
	 */
	bool bHealing;
	float HealingRate;
	float AmountToHeal;
	UPROPERTY(EditAnywhere, Replicated)
	TObjectPtr<UNiagaraSystem> HealingEffect;

	/**
	 * Shield buff
	 */
	bool bReplenishingShield;
	float ShieldReplenishRate;
	float ShieldReplenishAmount;
	UPROPERTY(EditAnywhere, Replicated)
	TObjectPtr<UNiagaraSystem> ShieldEffect;

	/**
	 * Speed buff
	 */
	FTimerHandle SpeedBuffTimer;
	void ResetSpeeds();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	UPROPERTY(EditAnywhere, Replicated)
	TObjectPtr<UNiagaraSystem> SpeedEffect;
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

	/**
	 * Jump buff
	 */
	FTimerHandle JumpBuffTimer;
	void ResetJump();
	float InitialJumpVelocity;
	UPROPERTY(EditAnywhere, Replicated)
	TObjectPtr<UNiagaraSystem> JumpEffect;
	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float JumpVelocity);

public:

	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);

	void SetInitialJumpVelocity(float Velocity);
	
};
