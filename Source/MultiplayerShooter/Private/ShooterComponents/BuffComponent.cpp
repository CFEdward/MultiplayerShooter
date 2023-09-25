// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterComponents/BuffComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "ShooterComponents/CombatComponent.h"


UBuffComponent::UBuffComponent() :
	bHealing(false), HealingRate(0.f), AmountToHeal(0.f), bReplenishingShield(false), ShieldReplenishRate(0.f), ShieldReplenishAmount(0.f),
	InitialBaseSpeed(0.f), InitialCrouchSpeed(0.f), InitialJumpVelocity(0.f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuffComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBuffComponent, HealingEffect);
	DOREPLIFETIME(UBuffComponent, ShieldEffect);
	DOREPLIFETIME(UBuffComponent, SpeedEffect);
	DOREPLIFETIME(UBuffComponent, JumpEffect);
}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);
	ShieldRampUp(DeltaTime);
}

void UBuffComponent::SetInitialSpeeds(const float BaseSpeed, const float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}

void UBuffComponent::SetInitialJumpVelocity(const float Velocity)
{
	InitialJumpVelocity = Velocity;
}

void UBuffComponent::MulticastSpawnCharacterEffect_Implementation(UNiagaraSystem* Effect)
{
	if (Character)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(Effect, Character->GetMesh(), FName("AttachPoint"), Character->GetActorLocation(), Character->GetActorRotation(), EAttachLocation::KeepWorldPosition, true);
	}
}

void UBuffComponent::Heal(const float HealAmount, const float HealingTime)
{
	bHealing = true;
	AmountToHeal += HealAmount;
	HealingRate = HealAmount / HealingTime;

	if (HealingEffect)
	{
		MulticastSpawnCharacterEffect(HealingEffect);
	}
}

void UBuffComponent::HealRampUp(const float DeltaTime)
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

void UBuffComponent::ReplenishShield(const float ShieldAmount, const float ReplenishTime)
{
	bReplenishingShield = true;
	ShieldReplenishAmount += ShieldAmount;
	ShieldReplenishRate = ShieldAmount / ReplenishTime;

	if (ShieldEffect)
	{
		MulticastSpawnCharacterEffect(ShieldEffect);
	}
	
}

void UBuffComponent::ShieldRampUp(const float DeltaTime)
{
	if (!bReplenishingShield || Character == nullptr || Character->IsElimmed()) return;

	const float ReplenishThisFrame = ShieldReplenishRate * DeltaTime;
	Character->SetShield(FMath::Clamp(Character->GetShield() + ReplenishThisFrame, 0.f, Character->GetMaxShield()));
	Character->UpdateHUDShield();
	ShieldReplenishAmount -= ReplenishThisFrame;
	
	if (ShieldReplenishAmount <= 0.f || Character->GetShield() >= Character->GetMaxShield())
	{
		bReplenishingShield = false;
		ShieldReplenishAmount = 0.f;
	}
}

void UBuffComponent::BuffSpeed(const float BuffBaseSpeed, const float BuffCrouchSpeed, const float BuffTime)
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(SpeedBuffTimer, this, &ThisClass::ResetSpeeds, BuffTime);
	
	MulticastSpeedBuff(BuffBaseSpeed, BuffCrouchSpeed);
}

void UBuffComponent::ResetSpeeds()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	
	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);
}
	
void UBuffComponent::MulticastSpeedBuff_Implementation(const float BaseSpeed, const float CrouchSpeed)
{
	if (BaseSpeed > Character->GetCharacterMovement()->MaxWalkSpeed && SpeedEffect)
	{
		MulticastSpawnCharacterEffect(SpeedEffect);
	}
	
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	if (Character->GetCombat())
	{
		Character->GetCombat()->SetWalkSpeeds(BaseSpeed, CrouchSpeed);
	}
}

void UBuffComponent::BuffJump(const float BuffJumpVelocity, const float BuffTime)
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(JumpBuffTimer, this, &ThisClass::ResetJump, BuffTime);

	MulticastJumpBuff(BuffJumpVelocity);
}

void UBuffComponent::MulticastJumpBuff_Implementation(const float JumpVelocity)
{
	if (JumpVelocity > Character->GetCharacterMovement()->JumpZVelocity && JumpEffect)
	{
		MulticastSpawnCharacterEffect(JumpEffect);
	}
	
	Character->GetCharacterMovement()->JumpZVelocity = JumpVelocity;
}

void UBuffComponent::ResetJump()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	
	MulticastJumpBuff(InitialJumpVelocity);
}