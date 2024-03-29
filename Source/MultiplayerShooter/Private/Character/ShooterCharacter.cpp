// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "GameMode/ShooterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/MultiplayerShooter.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PlayerController/ShooterPlayerController.h"
#include "PlayerState/ShooterPlayerState.h"
#include "ShooterComponents/BuffComponent.h"
#include "ShooterComponents/CombatComponent.h"
#include "Weapon/Weapon.h"
#include "Sound/SoundCue.h"
#include "Weapon/WeaponTypes.h"


AShooterCharacter::AShooterCharacter() :
	bDisableGameplay(false),
	bDrawPhysAssets(false),
	bShouldStopReload(false),
	CameraThreshold(200.0f),
	TurnThreshold(0.5f),
	TimeSinceLastMovementReplication(0.0f),
	MaxHealth(100.0f),
	Health(MaxHealth),
	MaxShield(100.f),
	Shield(0.f),
	bElimmed(false),
	ElimDelay(3.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttachedGrenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 850.0f, 0.0f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	/**
	 * Hit boxes for server-side rewind
	 *
	{
		Head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
		Head->SetupAttachment(GetMesh(), FName("head"));
		Head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
		Pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
		Pelvis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
		Spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
		Spine_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
		Spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
		Spine_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		UpperArm_L = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
		UpperArm_L->SetupAttachment(GetMesh(), FName("upperarm_l"));
		UpperArm_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		UpperArm_R = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
		UpperArm_R->SetupAttachment(GetMesh(), FName("upperarm_r"));
		UpperArm_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		LowerArm_L = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
		LowerArm_L->SetupAttachment(GetMesh(), FName("lowerarm_l"));
		LowerArm_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		LowerArm_R = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
		LowerArm_R->SetupAttachment(GetMesh(), FName("lowerarm_r"));
		LowerArm_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Hand_L = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
		Hand_L->SetupAttachment(GetMesh(), FName("hand_l"));
		Hand_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Hand_R = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
		Hand_R->SetupAttachment(GetMesh(), FName("hand_r"));
		Hand_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
		Backpack->SetupAttachment(GetMesh(), FName("backpack"));
		Backpack->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Blanket = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
		Blanket->SetupAttachment(GetMesh(), FName("backpack"));
		Blanket->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Thigh_L = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
		Thigh_L->SetupAttachment(GetMesh(), FName("thigh_l"));
		Thigh_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Thigh_R = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
		Thigh_R->SetupAttachment(GetMesh(), FName("thigh_r"));
		Thigh_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Calf_L = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
		Calf_L->SetupAttachment(GetMesh(), FName("calf_l"));
		Calf_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Calf_R = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
		Calf_R->SetupAttachment(GetMesh(), FName("calf_r"));
		Calf_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Foot_L = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
		Foot_L->SetupAttachment(GetMesh(), FName("foot_l"));
		Foot_L->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		Foot_R = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
		Foot_R->SetupAttachment(GetMesh(), FName("foot_r"));
		Foot_R->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	*/
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShooterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AShooterCharacter, Health);
	DOREPLIFETIME(AShooterCharacter, Shield);
	DOREPLIFETIME(AShooterCharacter, bDisableGameplay);
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}
	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}
	
	if (GetMesh())
	{
		HitCapsuleConstruction();
	}
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	HideCharacterIfCameraClose();
	PollInit();
}

void AShooterCharacter::RotateInPlace(const float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AShooterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AShooterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AShooterCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShooterCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &AShooterCharacter::GrenadeButtonPressed);
	PlayerInputComponent->BindAction("SwapWeapons", IE_Pressed, this, &AShooterCharacter::SwapWeaponsPressed);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);
}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
}

void AShooterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.0f;
}

void AShooterCharacter::DropWeapons() const
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
	}
	if (Combat && Combat->SecondaryWeapon)
	{
		Combat->SecondaryWeapon->Dropped();
	}
}

void AShooterCharacter::Elim()
{
	DropWeapons();
	MulticastElim();
	GetWorldTimerManager().SetTimer(ElimTimer, this, &ThisClass::ElimTimerFinished, ElimDelay);
}

void AShooterCharacter::Destroyed()
{
	Super::Destroyed();

	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}

	const AShooterGameMode* ShooterGameMode = Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this));
	const bool bMatchNotInProgress = ShooterGameMode && ShooterGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void AShooterCharacter::MulticastElim_Implementation()
{
	if (ShooterPlayerController)
	{
		ShooterPlayerController->SetHUDWeaponAmmo(0);
	}
	bElimmed = true;
	PlayElimMontage();

	// Start dissolve effect
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.0f);
	}
	StartDissolve();

	// Disable character movement
	bDisableGameplay = true;
	GetCharacterMovement()->DisableMovement();
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	
	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Spawn elim bot
	if (ElimBotEffect)
	{
		const FVector ElimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.0f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ElimBotEffect,
			ElimBotSpawnPoint, 
			GetActorRotation()
		);
	}
	if (ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, ElimBotSound, GetActorLocation());
	}
}

void AShooterCharacter::ElimTimerFinished()
{
	if (AShooterGameMode* ShooterGameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>())
	{
		ShooterGameMode->RequestRespawn(this, Controller);
	}
}

void AShooterCharacter::MoveForward(const float Value)
{
	if (bDisableGameplay) return;
	
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(const float Value)
{
	if (bDisableGameplay) return;
	
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::Turn(const float Value)
{
	AddControllerYawInput(Value);
}

void AShooterCharacter::LookUp(const float Value)
{
	AddControllerPitchInput(Value);
}

void AShooterCharacter::EquipButtonPressed()
{
	if (bDisableGameplay) return;
	
	if (Combat && OverlappingWeapon)
	{
		ServerEquipButtonPressed();
	}
}

void AShooterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat && OverlappingWeapon)
	{
		if (OverlappingWeapon->GetWeaponState() == EWeaponState::EWS_Initial) OverlappingWeapon->OnEquip.Broadcast();
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void AShooterCharacter::SwapWeaponsPressed()
{
	if (bDisableGameplay) return;
	
	if (Combat && Combat->ShouldSwapWeapons())
	{
		ServerSwapWeaponPressed();
	}
}

void AShooterCharacter::ServerSwapWeaponPressed_Implementation()
{
	if (Combat && Combat->ShouldSwapWeapons())
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); AnimInstance->Montage_IsPlaying(ReloadMontage))
		{
			AnimInstance->Montage_Stop(.5f, ReloadMontage);
			bShouldStopReload = true;
		}
		Combat->SwapWeapons();
		bShouldStopReload = false;
	}
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;
	
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay) return;
	
	if (Combat)
	{
		Combat->Reload();
	}
}

void AShooterCharacter::AimButtonPressed()
{
	if (bDisableGameplay) return;
	
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void AShooterCharacter::AimButtonReleased()
{
	if (bDisableGameplay) return;
	
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void AShooterCharacter::FireButtonPressed()
{
	if (bDisableGameplay) return;
	
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void AShooterCharacter::FireButtonReleased()
{
	if (bDisableGameplay) return;
	
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void AShooterCharacter::GrenadeButtonPressed()
{
	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

float AShooterCharacter::CalculateSpeed() const
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.0f;
	return Velocity.Size();
}

void AShooterCharacter::AimOffset(const float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;

	const float Speed = CalculateSpeed();
	const bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.0f && !bIsInAir)	// standing still, not jumping
	{
		bRotateRootBone = true;
		const FRotator CurrentAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(
			CurrentAimRotation,
			StartingAimRotation
		);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.0f || bIsInAir)	// running or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		AO_Yaw = 0.0f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void AShooterCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.0f && !IsLocallyControlled())
	{
		// map Pitch from [270 -> 360) to [-90 -> 0)
		const FVector2D InRange(270.0f, 360.0f);
		const FVector2D OutRange(-90.0f, 0.0f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void AShooterCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	bRotateRootBone = false;

	if (const float Speed = CalculateSpeed(); Speed > 0.0f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;
	
	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void AShooterCharacter::Jump()
{
	if (bDisableGameplay) return;
	
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AShooterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (bElimmed) return;

	float DamageToHealth = Damage;
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
			Shield = 0.f;
		}
	}
	
	Health = FMath::Clamp(Health - DamageToHealth, 0.0f, MaxHealth);
	
	UpdateHUDHealth();
	UpdateHUDShield();
	PlayHitReactMontage();

	if (Health == 0.0f)
	{
		if (AShooterGameMode* ShooterGameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>())
		{
			ShooterPlayerController =
				ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
			AShooterPlayerController* AttackerController = Cast<AShooterPlayerController>(InstigatorController);
			ShooterGameMode->PlayerEliminated(this, ShooterPlayerController, AttackerController);
		}
	}
}

void AShooterCharacter::PlayFireMontage(const bool bAiming) const
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		const FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AShooterCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		// Using this delegate to make sure the combat state is returned to unoccupied in case ReloadMontage is interrupted
		FOnMontageEnded BlendOutDelegate;
		BlendOutDelegate.BindUObject(this, &AShooterCharacter::ReloadMontageInterrupted);
		AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, ReloadMontage);
		
		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
			
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;

		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;

		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");
			break;

		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;

		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("Rifle");
			break;

		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("Rifle");
			break;
			
		default: break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AShooterCharacter::ReloadMontageInterrupted(UAnimMontage* Montage, bool bInterrupted)
{
	if (bShouldStopReload)
	{
		Combat->FinishReloading();
		UE_LOG(LogTemp, Warning, TEXT("Reload Montage interrupted"));
	}
	Combat->CombatState = ECombatState::ECS_Unoccupied;
}

void AShooterCharacter::PlayElimMontage() const
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void AShooterCharacter::PlayThrowGrenadeMontage() const
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void AShooterCharacter::PlayHitReactMontage() const
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); AnimInstance && HitReactMontage && !AnimInstance->IsAnyMontagePlaying())
	{
		AnimInstance->Montage_Play(HitReactMontage);
		const FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AShooterCharacter::TurnInPlace(const float DeltaTime)
{
	if (AO_Yaw > 90.0f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.0f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.0f, DeltaTime, 4.0f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.0f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		}
	}
}

void AShooterCharacter::HideCharacterIfCameraClose() const
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void AShooterCharacter::OnRep_Health(const float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void AShooterCharacter::OnRep_Shield(const float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield)
	{
		PlayHitReactMontage();
	}
}

void AShooterCharacter::UpdateHUDHealth()
{
	ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
	
	if (ShooterPlayerController)
	{
		ShooterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void AShooterCharacter::UpdateHUDShield()
{
	ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
	
	if (ShooterPlayerController)
	{
		ShooterPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void AShooterCharacter::UpdateHUDAmmo()
{
	ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
	
	if (ShooterPlayerController && Combat && Combat->EquippedWeapon)
	{
		ShooterPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		ShooterPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}

void AShooterCharacter::SpawnDefaultWeapon()
{
	AShooterGameMode* ShooterGameMode = Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (ShooterGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

void AShooterCharacter::PollInit()
{
	if (ShooterPlayerController == nullptr)
	{
		ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
		if (ShooterPlayerController)
		{
			SpawnDefaultWeapon();
			UpdateHUDAmmo();
			UpdateHUDHealth();
			UpdateHUDShield();
		}
	}
	
	if (ShooterPlayerState == nullptr)
	{
		ShooterPlayerState = GetPlayerState<AShooterPlayerState>();
		if (ShooterPlayerState)
		{
			ShooterPlayerState->AddToScore(0.0f);
			ShooterPlayerState->AddToDefeats(0);
		}
	}
}

void AShooterCharacter::UpdateDissolveMaterial(const float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void AShooterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ThisClass::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void AShooterCharacter::HitCapsuleConstruction()
{
	if (GetMesh() == nullptr) return;

	for (auto x : GetMesh()->GetPhysicsAsset()->SkeletalBodySetups)
	{
		auto BName = x->BoneName;
		auto BoneWorldTransform = GetMesh()->GetBoneTransform(GetMesh()->GetBoneIndex(BName));
		for (auto y : x->AggGeom.SphylElems)
		{
			auto LocTransform = y.GetTransform();
			auto WorldTransform = LocTransform * BoneWorldTransform;

			FPhysAssetInformation AssetInfo;
			AssetInfo.BoneName = BName;
			AssetInfo.HalfHeight = y.Length / 2 + y.Radius;
			AssetInfo.Radius = y.Radius;
			AssetInfo.BoneWorldTransform = WorldTransform;
			PhysAssetInfo.Add(AssetInfo);
		}
	}

	for (int i = 0; i < PhysAssetInfo.Num(); i++)
	{
		SetupHitCapsule(PhysAssetInfo[i]);
	}
}

void AShooterCharacter::SetupHitCapsule(FPhysAssetInformation PhysicsAssetInfo)
{
	UCapsuleComponent* HitCapsule = NewObject<UCapsuleComponent>(this, UCapsuleComponent::StaticClass(), PhysicsAssetInfo.BoneName, RF_Transient);
	HitCapsule->SetupAttachment(GetMesh(), PhysicsAssetInfo.BoneName);
	HitCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCapsule->SetWorldTransform(PhysicsAssetInfo.BoneWorldTransform);
	HitCapsule->SetCapsuleHalfHeight(PhysicsAssetInfo.HalfHeight);
	HitCapsule->SetCapsuleRadius(PhysicsAssetInfo.Radius);

	HitCapsule->RegisterComponent();
	HitCapsuleBones.Add(HitCapsule);
	HitCollisionCapsule.Add(PhysicsAssetInfo.BoneName, HitCapsule);
}

void AShooterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(false);
		}
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void AShooterCharacter::OnRep_OverlappingWeapon(const AWeapon* LastWeapon) const
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

bool AShooterCharacter::IsWeaponEquipped() const
{
	return (Combat && Combat->EquippedWeapon);
}

bool AShooterCharacter::IsAiming() const
{
	return (Combat && Combat->bAiming);
}

bool AShooterCharacter::IsLocallyReloading() const
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}

AWeapon* AShooterCharacter::GetEquippedWeapon() const
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector AShooterCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

ECombatState AShooterCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}
