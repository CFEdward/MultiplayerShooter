// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

class UTexture2D;
class UCharacterOverlay;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:

	UPROPERTY()
	UTexture2D* CrosshairsCenter;
	UPROPERTY()
	UTexture2D* CrosshairsLeft;
	UPROPERTY()
	UTexture2D* CrosshairsRight;
	UPROPERTY()
	UTexture2D* CrosshairsTop;
	UPROPERTY()
	UTexture2D* CrosshairsBottom;
	
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

	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	UCharacterOverlay* CharacterOverlay;

protected:

	virtual void BeginPlay() override;
	void AddCharacterOverlay();

private:

	FHUDPackage HUDPackage;

	void DrawCrosshair(
		UTexture2D* Texture,
		const FVector2D ViewportCenter,
		const FVector2D Spread,
		const FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.0f;

public:

	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
	
};
