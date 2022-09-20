// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/ShooterPlayerState.h"

#include "Character/ShooterCharacter.h"
#include "PlayerController/ShooterPlayerController.h"

void AShooterPlayerState::AddToScore(float ScoreAmount)
{
	Score += ScoreAmount;	
}

void AShooterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<AShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(Score);
		}
	}
}
