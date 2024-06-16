// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Flag.h"

#include "Character/ShooterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "ShooterComponents/CombatComponent.h"

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

void AFlag::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	FlagPoleMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	ShooterOwnerCharacter = nullptr;
	ShooterOwnerController = nullptr;
}

void AFlag::OnEquipped()
{
	ShowPickupWidget(false);
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	FlagPoleMesh->SetSimulatePhysics(false);
	FlagPoleMesh->SetEnableGravity(false);
	FlagPoleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlagPoleMesh->GetOwner()->SetActorScale3D(FVector(1.f));
	
	//FlagMesh->SetSimulatePhysics(false);
	//FlagMesh->SetEnableGravity(false);
	//FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//FlagPoleMesh->GetOwner()->
	
	SetReplicateMovement(false);
}

void AFlag::OnDropped()
{
	if (HasAuthority())
	{
		GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	FlagPoleMesh->GetOwner()->SetActorScale3D(FVector(1.5f));
	FlagPoleMesh->SetSimulatePhysics(true);
	FlagPoleMesh->SetEnableGravity(true);
	FlagPoleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FlagPoleMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	FlagPoleMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	FlagPoleMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	FlagPoleMesh->AddImpulse(GetActorRightVector() * DropWeaponImpulse);
	
	//FlagMesh->SetSimulatePhysics(true);
	//FlagMesh->SetEnableGravity(true);
	//FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
	SetReplicateMovement(true);
	//WeaponMesh->AddImpulse(GetActorRightVector() * DropWeaponImpulse);
	

}
