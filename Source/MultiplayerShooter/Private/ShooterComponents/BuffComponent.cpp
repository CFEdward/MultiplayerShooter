// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterComponents/BuffComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "ShooterComponents/CombatComponent.h"


UBuffComponent::UBuffComponent() :
	bHealing(false),
	HealingRate(0.f),
	AmountToHeal(0.f),
	InitialBaseSpeed(0.f),
	InitialCrouchSpeed(0.f),
	InitialJumpVelocity(0.f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuffComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBuffComponent, HealingEffect);
	DOREPLIFETIME(UBuffComponent, SpeedEffect);
	DOREPLIFETIME(UBuffComponent, JumpEffect);
}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
	
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

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);	
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

	MulticastSpawnCharacterEffect(HealingEffect);
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
	if (BaseSpeed > Character->GetCharacterMovement()->MaxWalkSpeed)
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
	if (JumpVelocity > Character->GetCharacterMovement()->JumpZVelocity)
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