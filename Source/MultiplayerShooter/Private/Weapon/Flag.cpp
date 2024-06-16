// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Flag.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

AFlag::AFlag()
{
	FlagPoleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagPoleMesh"));
	SetRootComponent(FlagPoleMesh);
	FlagMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlagMesh"));
	FlagMesh->SetupAttachment(FlagPoleMesh);

	GetAreaSphere()->SetupAttachment(FlagPoleMesh);
	GetPickupWidget()->SetupAttachment(FlagPoleMesh);
}
