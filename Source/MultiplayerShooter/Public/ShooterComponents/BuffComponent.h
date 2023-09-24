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

	void Heal(float HealAmount, float HealingTime);
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	
	virtual void BeginPlay() override;

	void HealRampUp(float DeltaTime);

private:

	UPROPERTY()
	TObjectPtr<AShooterCharacter> Character;

	bool bHealing;
	float HealingRate;
	float AmountToHeal;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> HealingEffect;
	
};
