// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

class AShooterHUD;
class UCharacterOverlay;
class AShooterGameMode;

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AShooterPlayerController();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetHUDHealth(const float Health, const float MaxHealth);
	void SetHUDShield(const float Shield, const float MaxShield);
	void SetHUDScore(const float Score);
	void SetHUDDefeats(const int32 Defeats);
	void SetHUDWeaponAmmo(const int32 Ammo);
	void SetHUDCarriedAmmo(const int32 Ammo);
	void SetHUDMatchCountdown(const float CountdownTime);
	void SetHUDAnnouncementCountdown(const float CountdownTime);
	void SetHUDSniperScope(const bool bIsAiming);
	void SetHUDGrenades(int32 Grenades);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	virtual float GetServerTime();	// Synced with server world clock
	virtual void ReceivedPlayer() override;	// Sync with server clock as soon as possible
	void OnMatchStateSet(const FName State);
	
protected:
	
	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInit();
	void HandleMatchHasStarted();
	void HandleCooldown();

	/**
	 * Sync time between client and server
	 */

	// Requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(const float TimeOfClientRequest);

	// Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(const float TimeOfClientRequest, const float TimeServerReceivedClientRequest);
	
	float ClientServerDelta;	// Difference between client and server time

	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency;

	float TimeSyncRunningTime;
	void CheckTimeSync(const float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(
		const FName StateOfMatch,
		const float Warmup,
		const float Match,
		const float Cooldown,
		const float StartingTime
	);

	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaTime);
	
private:

	UPROPERTY()
	TObjectPtr<AShooterHUD> ShooterHUD;

	UPROPERTY()
	TObjectPtr<AShooterGameMode> ShooterGameMode;

	float LevelStartingTime;
	float MatchTime;
	float WarmupTime;
	float CooldownTime;
	uint32 CountdownInt;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	TObjectPtr<UCharacterOverlay> CharacterOverlay;

	float HUDHealth;
	bool bInitializeHealth;
	float HUDMaxHealth;

	float HUDShield;
	bool bInitializeShield;
	float HUDMaxShield;
	
	float HUDScore;
	bool bInitializeScore;
	int32 HUDDefeats;
	bool bInitializeDefeats;
	int32 HUDGrenades;
	bool bInitializeGrenades;
	float HUDCarriedAmmo;
	bool bInitializeCarriedAmmo;
	float HUDWeaponAmmo;
	bool bInitializeWeaponAmmo;

	float HighPingRunningTime;
	float PingAnimationRunningTime;
	UPROPERTY(EditAnywhere)
	float HighPingDuration;
	UPROPERTY(EditAnywhere)
	float CheckPingFrequency;
	UPROPERTY(EditAnywhere)
	float HighPingThreshold;
	
};
