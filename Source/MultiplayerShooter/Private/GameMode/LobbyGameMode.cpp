// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"

#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameStateBase.h"


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	const int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		const UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check(Subsystem);

		if (NumberOfPlayers == Subsystem->DesiredNumPublicConnections)
		{
			if (UWorld* World = GetWorld())
			{
				bUseSeamlessTravel = true;
				FString MatchType = Subsystem->DesiredMatchType;
				if (MatchType == "FreeForAll")
				{
					World->ServerTravel(FString("/Game/Maps/ShooterMap?listen"));
				}
				else if (MatchType == "Teams")
				{
					World->ServerTravel(FString("/Game/Maps/TeamsMap?listen"));
				}
				else if (MatchType == "CaptureTheFlag")
				{
					World->ServerTravel(FString("/Game/Maps/CaptureTheFlagMap?listen"));
				}
			}
		}
	}
}
