// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ShooterHUD.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "GameFramework/PlayerController.h"
#include "HUD/Announcement.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/ElimAnnouncement.h"
#include "HUD/SniperScope.h"


AShooterHUD::AShooterHUD() :
	HUDPackage(),
	CrosshairSpreadMax(16.0f),
	ElimAnnouncementTime(2.5f)
{
	
}

void AShooterHUD::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShooterHUD::AddCharacterOverlay()
{
	if (APlayerController* PlayerController = GetOwningPlayerController(); PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void AShooterHUD::AddAnnouncement()
{
	if (APlayerController* PlayerController = GetOwningPlayerController(); PlayerController && AnnouncementClass)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void AShooterHUD::AddElimAnnouncement(const FString& Attacker, const FString& Victim)
{
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayerController() : OwningPlayer.Get();
	if (OwningPlayer && ElimAnnouncementClass)
	{
		if (UElimAnnouncement* ElimAnnouncementWidget = CreateWidget<UElimAnnouncement>(OwningPlayer, ElimAnnouncementClass))
		{
			ElimAnnouncementWidget->SetElimAnnouncementText(Attacker, Victim);
			ElimAnnouncementWidget->AddToViewport();
			for (const auto Msg : ElimMessages)
			{
				if (Msg && Msg->AnnouncementBox)
				{
					if (UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Msg->AnnouncementBox))
					{
						const FVector2D Position = CanvasSlot->GetPosition();
						const FVector2D NewPosition(CanvasSlot->GetPosition().X, Position.Y - CanvasSlot->GetSize().Y);
						CanvasSlot->SetPosition(NewPosition);
					}
				}
			}
			ElimMessages.Add(ElimAnnouncementWidget);

			FTimerHandle ElimMsgTimer;
			FTimerDelegate ElimMsgDelegate;
			ElimMsgDelegate.BindUFunction(this, FName("ElimAnnouncementTimerFinished"), ElimAnnouncementWidget);
			GetWorldTimerManager().SetTimer(ElimMsgTimer, ElimMsgDelegate, ElimAnnouncementTime, false);
		}
	}
}

void AShooterHUD::ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove)
{
	if (MsgToRemove)
	{
		MsgToRemove->RemoveFromParent();
	}
}

void AShooterHUD::AddSniperScope()
{
	if (APlayerController* PlayerController = GetOwningPlayerController(); PlayerController && SniperScopeClass)
	{
		SniperScope = CreateWidget<USniperScope>(PlayerController, SniperScopeClass);
		SniperScope->AddToViewport();
	}
}

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (GEngine)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);

		const float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;
		
		if (HUDPackage.CrosshairsCenter)
		{
			const FVector2D Spread(0.0f, 0.0f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			const FVector2D Spread(-SpreadScaled, 0.0f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsRight)
		{
			const FVector2D Spread(SpreadScaled, 0.0f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsTop)
		{
			const FVector2D Spread(0.0f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			const FVector2D Spread(0.0f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
	}
}

void AShooterHUD::DrawCrosshair(
	UTexture2D* Texture,
	const FVector2D ViewportCenter,
	const FVector2D Spread,
	const FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.0f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.0f) + Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.0f,
		0.0f,
		1.0f,
		1.0f,
		CrosshairColor
	);
}
