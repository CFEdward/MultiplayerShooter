// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReturnToMainMenu.generated.h"

class UMultiplayerSessionsSubsystem;
class UButton;
class APlayerController;

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API UReturnToMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	void MenuSetup();
	void MenuTearDown();

protected:

	virtual bool Initialize() override;

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

private:

	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;
	
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnButton;
	UFUNCTION()
	void ReturnButtonClicked();
};
