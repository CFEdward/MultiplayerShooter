// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureTheFlag/FlagZone.h"

#include "Components/SphereComponent.h"
#include "GameMode/CaptureTheFlagGameMode.h"
#include "Weapon/Flag.h"

AFlagZone::AFlagZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneSphere"));
	SetRootComponent(ZoneSphere);
}

void AFlagZone::BeginPlay()
{
	Super::BeginPlay();

	ZoneSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
}

void AFlagZone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AFlag* OverlappingFlag = Cast<AFlag>(OtherActor); OverlappingFlag && OverlappingFlag->GetTeam() != Team)
	{
		if (const ACaptureTheFlagGameMode* GameMode = GetWorld()->GetAuthGameMode<ACaptureTheFlagGameMode>())
		{
			GameMode->FlagCaptured(OverlappingFlag, this);
		}
		OverlappingFlag->ResetFlag();
	}
}
