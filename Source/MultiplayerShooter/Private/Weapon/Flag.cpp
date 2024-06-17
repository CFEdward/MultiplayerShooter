// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Flag.h"

#include "Character/ShooterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AFlag::AFlag()
{
	bReplicates = true;
	SetReplicateMovement(true);
	
	FlagPoleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagPoleMesh"));
	FlagPoleMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	FlagPoleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(FlagPoleMesh);
	FlagMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlagMesh"));
	FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlagMesh->SetupAttachment(FlagPoleMesh);

	GetAreaSphere()->SetupAttachment(RootComponent);
	GetAreaSphere()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetPickupWidget()->SetupAttachment(RootComponent);
}

void AFlag::BeginPlay()
{
	Super::BeginPlay();

	InitialTransform = GetActorTransform();
}

void AFlag::OnEquipped()
{
	ShowPickupWidget(false);

	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlagPoleMesh->SetSimulatePhysics(false);
	FlagPoleMesh->SetEnableGravity(false);
	FlagPoleMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FlagPoleMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	//FlagPoleMesh->SetRelativeScale3D(FVector(1.f));
}

void AFlag::Dropped()
{
	FVector LocationToMove = FVector::Zero();
	if (AShooterCharacter* FlagBearer = Cast<AShooterCharacter>(GetOwner()))
	{
		FlagBearer->SetHoldingTheFlag(false);
		FlagBearer->SetOverlappingWeapon(nullptr);
		LocationToMove = FlagBearer->GetActorLocation();
		if (HasAuthority()) FlagBearer->GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	//FlagPoleMesh->SetRelativeScale3D(FVector(1.5f));
	if (!HasAuthority()) return;

	SetReplicateMovement(true);
	const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	FlagPoleMesh->DetachFromComponent(DetachRules);
	SetWeaponState(EWeaponState::EWS_Dropped);
	
	SetOwner(nullptr);
	ShooterOwnerCharacter = nullptr;
	ShooterOwnerController = nullptr;

	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetAreaSphere()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	FlagPoleMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	FlagPoleMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	FlagPoleMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	if (LocationToMove != FVector::Zero()) SetActorLocation(FVector(LocationToMove.X, LocationToMove.Y, LocationToMove.Z - 88.f));
}

void AFlag::OnDropped()
{

	// if (HasAuthority())
	// {
	// 	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// }
	// //FlagPoleMesh->SetRelativeScale3D(FVector(1.5f));
	// FlagPoleMesh->SetRelativeScale3D(FVector(1.5f));
	// FlagPoleMesh->SetSimulatePhysics(true);
	// FlagPoleMesh->SetEnableGravity(true);
	// FlagPoleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// SetReplicateMovement(true); // new
	// FlagPoleMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	// FlagPoleMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// FlagPoleMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	// FlagPoleMesh->AddImpulse(GetActorRightVector() * DropWeaponImpulse);
	
	//FlagMesh->SetSimulatePhysics(true);
	//FlagMesh->SetEnableGravity(true);
	//FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//WeaponMesh->AddImpulse(GetActorRightVector() * DropWeaponImpulse);
}

void AFlag::ResetFlag()
{
	if (AShooterCharacter* FlagBearer = Cast<AShooterCharacter>(GetOwner()))
	{
		FlagBearer->SetHoldingTheFlag(false);
		FlagBearer->SetOverlappingWeapon(nullptr);
		if (HasAuthority()) FlagBearer->GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	if (!HasAuthority()) return;

	SetReplicateMovement(true);
	const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	FlagPoleMesh->DetachFromComponent(DetachRules);
	SetWeaponState(EWeaponState::EWS_Initial);
	
	SetOwner(nullptr);
	ShooterOwnerCharacter = nullptr;
	ShooterOwnerController = nullptr;

	SetActorTransform(InitialTransform);
	
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetAreaSphere()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}
