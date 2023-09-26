// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/Pickup.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/ShooterCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Weapon/WeaponTypes.h"


APickup::APickup() :
	BaseTurnRate(45.f),
	BindOverlapTime(.25f)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(150.f);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapSphere->AddLocalOffset(FVector(0.f, 0.f, 85.f));

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(OverlapSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupMesh->SetRelativeScale3D(FVector(4.f));
	PickupMesh->SetRenderCustomDepth(true);
	PickupMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);

	PickupEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickupEffectComponent"));
	PickupEffectComponent->SetupAttachment(RootComponent);
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(BindOverlapTimer, this, &ThisClass::BindOverlapTimerFinished, BindOverlapTime);
	}
}

void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PickupMesh)
	{
		PickupMesh->AddWorldRotation(FRotator(0.f, BaseTurnRate * DeltaTime, 0.f));
	}
}

void APickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor))
	{
		OnOverlap(ShooterCharacter);
	}
}

void APickup::OnOverlap(AShooterCharacter* ShooterCharacter)
{
	
}

void APickup::BindOverlapTimerFinished()
{
	if (HasAuthority())
	{
		GetWorldTimerManager().ClearTimer(BindOverlapTimer);
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
		
		TArray<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors, AShooterCharacter::StaticClass());

		if (OverlappingActors.Num() > 0)
		{
			if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetClosestPlayerToPickup(OverlappingActors)))
			{
				UE_LOG(LogTemp, Warning, TEXT("Closest player to pickup was %s"), *ShooterCharacter->GetName());
				OnOverlap(ShooterCharacter);
			}
		}
		
		/*for (const auto OverlappingActor : OverlappingActors)
		{
			if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OverlappingActor))
			{
				OnOverlap(ShooterCharacter);
			}
		}*/
	}
}

AActor* APickup::GetClosestPlayerToPickup(const TArray<AActor*>& Players) const
{
	AActor* ClosestPlayer = nullptr;
	float DistanceFromNearestPlayer = TNumericLimits<float>::Max();

	for (AActor* PlayerToCheck : Players)
	{
		if (PlayerToCheck)
		{
			const float DistanceFromPlayerToCheck = (GetActorLocation() - PlayerToCheck->GetActorLocation()).SizeSquared();
			if (DistanceFromPlayerToCheck < DistanceFromNearestPlayer)
			{
				ClosestPlayer = PlayerToCheck;
				DistanceFromNearestPlayer = DistanceFromPlayerToCheck;
			}
		}
	}

	return ClosestPlayer;
}

void APickup::Destroyed()
{
	Super::Destroyed();

	OverlapSphere->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnSphereOverlap);

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation(), GetActorRotation());
	}
}
