// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/ShooterPlayerState.h"
#include "Character/ShooterCharacter.h"
#include "PlayerController/ShooterPlayerController.h"
#include "Net/UnrealNetwork.h"


void AShooterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerState, Defeats);
}

void AShooterPlayerState::AddToScore(const float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	if (Character == nullptr) Character = Cast<AShooterCharacter>(GetPawn());
	//Character = Character == nullptr ? Cast<AShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		if (Controller == nullptr) Controller = Cast<AShooterPlayerController>(Character->Controller);
		//Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void AShooterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	if (Character == nullptr) Character = Cast<AShooterCharacter>(GetPawn());
	//Character = Character == nullptr ? Cast<AShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		if (Controller == nullptr) Controller = Cast<AShooterPlayerController>(Character->Controller);
		//Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void AShooterPlayerState::AddToDefeats(const int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	if (Character == nullptr) Character = Cast<AShooterCharacter>(GetPawn());
	//Character = Character == nullptr ? Cast<AShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		if (Controller == nullptr) Controller = Cast<AShooterPlayerController>(Character->Controller);
		//Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void AShooterPlayerState::OnRep_Defeats()
{
	if (Character == nullptr) Character = Cast<AShooterCharacter>(GetPawn());
	//Character = Character == nullptr ? Cast<AShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		if (Controller == nullptr) Controller = Cast<AShooterPlayerController>(Character->Controller);
		//Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}
