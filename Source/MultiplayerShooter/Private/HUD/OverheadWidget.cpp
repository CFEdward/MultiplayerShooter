// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;

	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;

	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;

	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}

	FString PawnName = InPawn->GetName();
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s, Name: %s"), *Role, *PawnName);
	SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}
