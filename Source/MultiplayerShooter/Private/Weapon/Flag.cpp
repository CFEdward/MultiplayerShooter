// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Flag.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

AFlag::AFlag()
{
	FlagPoleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagPoleMesh"));
	FlagPoleMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	FlagPoleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(FlagPoleMesh);
	FlagMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlagMesh"));
	FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlagMesh->SetupAttachment(FlagPoleMesh);

	GetAreaSphere()->SetupAttachment(FlagPoleMesh);
	GetPickupWidget()->SetupAttachment(FlagPoleMesh);
	
}
