// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterComponents/BuffComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/ShooterCharacter.h"


UBuffComponent::UBuffComponent() :
	bHealing(false),
	HealingRate(0.f),
	AmountToHeal(0.f)

{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);	
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	AmountToHeal += HealAmount;
	HealingRate = HealAmount / HealingTime;

	if (Character)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(HealingEffect, Character->GetMesh(), FName("AttachPoint"), Character->GetActorLocation(), Character->GetActorRotation(), EAttachLocation::KeepWorldPosition, true);
	}
}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsElimmed()) return;

	const float HealThisFrame = HealingRate * DeltaTime;
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();
	AmountToHeal -= HealThisFrame;
	
	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}
}
