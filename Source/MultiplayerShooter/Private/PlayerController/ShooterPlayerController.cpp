// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/ShooterPlayerController.h"

#include "Character/ShooterCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/ShooterHUD.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ShooterHUD = Cast<AShooterHUD>(GetHUD());
}

void AShooterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (const AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InPawn))
	{
		SetHUDHealth(ShooterCharacter->GetHealth(), ShooterCharacter->GetMaxHealth());
	}
}

void AShooterPlayerController::SetHUDHealth(const float Health, const float MaxHealth)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	
	if (ShooterHUD &&
		ShooterHUD->CharacterOverlay &&
		ShooterHUD->CharacterOverlay->HealthBar &&
		ShooterHUD->CharacterOverlay->HealthText)
	{
		const float HealthPercent = Health / MaxHealth;
		ShooterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		const FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		ShooterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void AShooterPlayerController::SetHUDScore(const float Score)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	if (ShooterHUD &&
		ShooterHUD->CharacterOverlay &&
		ShooterHUD->CharacterOverlay->ScoreAmount)
	{
		const FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		ShooterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void AShooterPlayerController::SetHUDDefeats(const int32 Defeats)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	if (ShooterHUD &&
		ShooterHUD->CharacterOverlay &&
		ShooterHUD->CharacterOverlay->DefeatsAmount)
	{
		const FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		ShooterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
}

void AShooterPlayerController::SetHUDWeaponAmmo(const int32 Ammo)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	if (ShooterHUD &&
		ShooterHUD->CharacterOverlay &&
		ShooterHUD->CharacterOverlay->WeaponAmmoAmount)
	{
		const FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		ShooterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AShooterPlayerController::SetHUDCarriedAmmo(const int32 Ammo)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	if (ShooterHUD &&
		ShooterHUD->CharacterOverlay &&
		ShooterHUD->CharacterOverlay->CarriedAmmoAmount)
	{
		const FString CarriedAmmoText = FString::Printf(TEXT("%d"), Ammo);
		ShooterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoText));
	}
}
