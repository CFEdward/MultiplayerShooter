// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SniperScope.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API USniperScope : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ScopeZoomIn;
	
};
