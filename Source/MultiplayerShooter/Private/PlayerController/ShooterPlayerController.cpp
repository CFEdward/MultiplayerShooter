// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/ShooterPlayerController.h"
#include "Character/ShooterCharacter.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameMode/ShooterGameMode.h"
#include "GameState/ShooterGameState.h"
#include "HUD/Announcement.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/ShooterHUD.h"
#include "HUD/SniperScope.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/ShooterPlayerState.h"
#include "ShooterComponents/CombatComponent.h"


AShooterPlayerController::AShooterPlayerController() :
	ClientServerDelta(0.f), TimeSyncFrequency(5.f), TimeSyncRunningTime(0.f), LevelStartingTime(0.f), MatchTime(0.f),
	WarmupTime(0.f), CooldownTime(0.f), CountdownInt(0), HUDHealth(0.f), bInitializeHealth(false),
	HUDMaxHealth(0.f), HUDShield(0.f), bInitializeShield(false), HUDMaxShield(0.f), HUDScore(0.f),
	bInitializeScore(false), HUDDefeats(0), bInitializeDefeats(false), HUDGrenades(0), bInitializeGrenades(false),
	HUDCarriedAmmo(0.f), bInitializeCarriedAmmo(false), HUDWeaponAmmo(0.f), bInitializeWeaponAmmo(false),
	HighPingRunningTime(0.f), PingAnimationRunningTime(0.f), HighPingDuration(5.f), CheckPingFrequency(20.f), HighPingThreshold(50.f)
{
	
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	ShooterHUD = Cast<AShooterHUD>(GetHUD());
	
	ServerCheckMatchState();
}

void AShooterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerController, MatchState);
}

void AShooterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
	CheckPing(DeltaTime);
}

void AShooterPlayerController::CheckPing(const float DeltaTime)
{
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
		if (PlayerState)
		{
			if (PlayerState->GetPingInMilliseconds() > HighPingThreshold)
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
			}
		}
		HighPingRunningTime = 0.f;
	}
	if (ShooterHUD
		&& ShooterHUD->CharacterOverlay
		&& ShooterHUD->CharacterOverlay->HighPingAnimation
		&& ShooterHUD->CharacterOverlay->IsAnimationPlaying(ShooterHUD->CharacterOverlay->HighPingAnimation))
	{
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

void AShooterPlayerController::CheckTimeSync(const float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.0f;
	}
}

void AShooterPlayerController::HighPingWarning()
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	
	if (ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->HighPingImage && ShooterHUD->CharacterOverlay->HighPingAnimation)
	{
		ShooterHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		ShooterHUD->CharacterOverlay->PlayAnimation(ShooterHUD->CharacterOverlay->HighPingAnimation, 0.f, 5);
	}
}

void AShooterPlayerController::StopHighPingWarning()
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	
	if (ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->HighPingImage && ShooterHUD->CharacterOverlay->HighPingAnimation)
	{
		ShooterHUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);
		if (ShooterHUD->CharacterOverlay->IsAnimationPlaying(ShooterHUD->CharacterOverlay->HighPingAnimation))
		{
			ShooterHUD->CharacterOverlay->StopAnimation(ShooterHUD->CharacterOverlay->HighPingAnimation);
		}
	}
}

void AShooterPlayerController::ServerCheckMatchState_Implementation()
{
	if (const AShooterGameMode* GameMode = Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}

void AShooterPlayerController::ClientJoinMidgame_Implementation(
	const FName StateOfMatch,
	const float Warmup,
	const float Match,
	const float Cooldown,
	const float StartingTime
)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);

	if (ShooterHUD && MatchState == MatchState::WaitingToStart)
	{
		ShooterHUD->AddAnnouncement();
	}
}

void AShooterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (const AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InPawn))
	{
		SetHUDHealth(ShooterCharacter->GetHealth(), ShooterCharacter->GetMaxHealth());
		SetHUDShield(ShooterCharacter->GetShield(), ShooterCharacter->GetMaxShield());
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
	else
	{
		bInitializeHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void AShooterPlayerController::SetHUDShield(const float Shield, const float MaxShield)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	
	if (ShooterHUD &&
		ShooterHUD->CharacterOverlay &&
		ShooterHUD->CharacterOverlay->ShieldBar &&
		ShooterHUD->CharacterOverlay->ShieldText)
	{
		const float ShieldPercent = Shield / MaxShield;
		ShooterHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		const FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		ShooterHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
	else
	{
		bInitializeShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
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
	else
	{
		bInitializeScore = true;
		HUDScore = Score;
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
	else
	{
		bInitializeDefeats = true;
		HUDDefeats = Defeats;
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
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
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
	else
	{
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
	}
}

void AShooterPlayerController::SetHUDMatchCountdown(const float CountdownTime)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	if (ShooterHUD &&
		ShooterHUD->CharacterOverlay &&
		ShooterHUD->CharacterOverlay->MatchCountdownText)
	{
		if (CountdownTime < 0.0f)
		{
			ShooterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		
		const int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		const int32 Seconds = CountdownTime - Minutes * 60;
		
		const FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		ShooterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AShooterPlayerController::SetHUDAnnouncementCountdown(const float CountdownTime)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	if (ShooterHUD &&
		ShooterHUD->Announcement &&
		ShooterHUD->Announcement->WarmupTime)
	{
		if (CountdownTime < 0.0f)
		{
			ShooterHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		
		const int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		const int32 Seconds = CountdownTime - Minutes * 60;
		
		const FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		ShooterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void AShooterPlayerController::SetHUDSniperScope(const bool bIsAiming)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (!ShooterHUD->SniperScope)
	{
		ShooterHUD->AddSniperScope();
	}
	if (ShooterHUD && ShooterHUD->SniperScope && ShooterHUD->SniperScope->ScopeZoomIn)
	{
		if (bIsAiming)
		{
			ShooterHUD->SniperScope->PlayAnimation(ShooterHUD->SniperScope->ScopeZoomIn);
		}
		else
		{
			ShooterHUD->SniperScope->PlayAnimation(
				ShooterHUD->SniperScope->ScopeZoomIn,
				0.0f,
				1,
				EUMGSequencePlayMode::Reverse
			);
		}
	}
}

void AShooterPlayerController::SetHUDGrenades(const int32 Grenades)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	if (ShooterHUD &&
		ShooterHUD->CharacterOverlay &&
		ShooterHUD->CharacterOverlay->GrenadesText)
	{
		const FString GrenadesString = FString::Printf(TEXT("%d"), Grenades);
		ShooterHUD->CharacterOverlay->GrenadesText->SetText(FText::FromString(GrenadesString));
	}
	else
	{
		bInitializeGrenades = true;
		HUDGrenades = Grenades;
	}
}

void AShooterPlayerController::SetHUDTime()
{
	ShooterGameMode = ShooterGameMode == nullptr ?
			Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this)) : ShooterGameMode;
	
	if (HasAuthority())
	{
		LevelStartingTime = ShooterGameMode->LevelStartingTime;
	}
	
	float TimeLeft = 0.0f;
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::Cooldown)
	{
		TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority())
	{
		if (ShooterGameMode)
		{
			SecondsLeft = FMath::CeilToInt(ShooterGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}
	
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	
	CountdownInt = SecondsLeft;
}

void AShooterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (ShooterHUD && ShooterHUD->CharacterOverlay)
		{
			CharacterOverlay = ShooterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializeHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitializeShield) SetHUDShield(HUDShield, HUDMaxShield);
				if (bInitializeScore) SetHUDScore(HUDScore);
				if (bInitializeDefeats) SetHUDDefeats(HUDDefeats);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);

				if (const AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn()); ShooterCharacter && ShooterCharacter->GetCombat())
				{
					if (bInitializeGrenades) SetHUDGrenades(ShooterCharacter->GetCombat()->GetGrenades());
				}
			}
		}
	}
}

void AShooterPlayerController::ServerRequestServerTime_Implementation(const float TimeOfClientRequest)
{
	const float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AShooterPlayerController::ClientReportServerTime_Implementation(
	const float TimeOfClientRequest,
	const float TimeServerReceivedClientRequest)
{
	const float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	const float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AShooterPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AShooterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AShooterPlayerController::OnMatchStateSet(const FName State)
{
	MatchState = State;
	
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AShooterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AShooterPlayerController::HandleMatchHasStarted()
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD)
	{
		if (ShooterHUD->CharacterOverlay == nullptr) ShooterHUD->AddCharacterOverlay();
		if (ShooterHUD->Announcement)
		{
			ShooterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AShooterPlayerController::HandleCooldown()
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD)
	{
		ShooterHUD->CharacterOverlay->RemoveFromParent();
		if (ShooterHUD->Announcement && ShooterHUD->Announcement->AnnouncementText && ShooterHUD->Announcement->InfoText)
		{
			ShooterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			const FString AnnouncementText("New Match Starts In: ");
			ShooterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			const AShooterGameState* ShooterGameState =
				Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
			const AShooterPlayerState* ShooterPlayerState = GetPlayerState<AShooterPlayerState>();
			if (ShooterGameState && ShooterPlayerState)
			{
				TArray<AShooterPlayerState*> TopPlayers = ShooterGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is no winner.");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == ShooterPlayerState)
				{
					InfoTextString = FString("You are the winner!");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win:\n");
					for (const auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}
				
				ShooterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	}

	if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
		ShooterCharacter && ShooterCharacter->GetCombat())
	{
		ShooterCharacter->bDisableGameplay = true;
		ShooterCharacter->GetCombat()->FireButtonPressed(false);
	}
}
