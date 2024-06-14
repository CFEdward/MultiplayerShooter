// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

class UElimAnnouncement;
class UTexture2D;
class UCharacterOverlay;
class UAnnouncement;
class USniperScope;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairsCenter;
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairsLeft;
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairsRight;
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairsTop;
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairsBottom;
	
	float CrosshairSpread;
	FLinearColor CrosshairsColor;
	
};

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

public:

	AShooterHUD();

	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<UUserWidget> CharacterOverlayClass;
	void AddCharacterOverlay();
	UPROPERTY()
	TObjectPtr<UCharacterOverlay> CharacterOverlay;
	
	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<UUserWidget> AnnouncementClass;
	void AddAnnouncement();
	UPROPERTY()
	TObjectPtr<UAnnouncement> Announcement;

	void AddElimAnnouncement(const FString& Attacker, const FString& Victim);

	UPROPERTY(EditAnywhere, Category = "Sniper")
	TSubclassOf<UUserWidget> SniperScopeClass;
	void AddSniperScope();
	UPROPERTY()
	TObjectPtr<USniperScope> SniperScope;
	
protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY()
	TObjectPtr<APlayerController> OwningPlayer;

	FHUDPackage HUDPackage;
	void DrawCrosshair(
		UTexture2D* Texture,
		const FVector2D ViewportCenter,
		const FVector2D Spread,
		const FLinearColor CrosshairColor
	);
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UElimAnnouncement> ElimAnnouncementClass;
	UPROPERTY(EditAnywhere)
	float ElimAnnouncementTime;
	UFUNCTION()
	void ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove);
	UPROPERTY()
	TArray<TObjectPtr<UElimAnnouncement>> ElimMessages;

public:

	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
	
};
