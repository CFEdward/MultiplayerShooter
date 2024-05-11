// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

class AShooterCharacter;
class AWeapon;
class AShooterPlayerController;

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location = FVector(0.f);

	UPROPERTY()
	FRotator Rotation = FRotator(0.f);

	UPROPERTY()
	FVector BoxExtent = FVector(0.f);
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time = 0.f;

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo = TMap<FName, FBoxInformation>();

	UPROPERTY()
	TObjectPtr<AShooterCharacter> Character = nullptr;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed = false;

	UPROPERTY()
	bool bHeadshot = false;
};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<AShooterCharacter>, uint32> HeadShots = TMap<TObjectPtr<AShooterCharacter>, uint32>();
	
	UPROPERTY()
	TMap<TObjectPtr<AShooterCharacter>, uint32> BodyShots = TMap<TObjectPtr<AShooterCharacter>, uint32>();
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	ULagCompensationComponent();
	friend class AShooterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ShowFramePackage(const FFramePackage& Package, const FColor& Color) const;

	/**
	 * Hitscan
	 */
	FServerSideRewindResult ServerSideRewind(
		AShooterCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		const float HitTime);
	/**
	 * Projectile
	 */
	FServerSideRewindResult ProjectileServerSideRewind(
		AShooterCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		const float HitTime);
	/**
	 * Shotgun
	 */
	FShotgunServerSideRewindResult ShotgunServerSideRewind(
		const TArray<AShooterCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		const float HitTime);

	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(
		AShooterCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		const float HitTime,
		AWeapon* DamageCauser);
	UFUNCTION(Server, Reliable)
	void ProjectileScoreRequest(
		AShooterCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		const float HitTime);
	UFUNCTION(Server, Reliable)
	void ShotgunServerScoreRequest(
		const TArray<AShooterCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		const float HitTime);
	
protected:
	
	virtual void BeginPlay() override;

	void SaveFramePackage();
	void SaveFramePackage(FFramePackage& Package);

	FFramePackage GetFrameToCheck(AShooterCharacter* HitCharacter, const float HitTime);
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, const float HitTime);
	
	void CacheBoxPositions(AShooterCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveHitBoxes(AShooterCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(AShooterCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(const AShooterCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);

	/**
	 * Hitscan
	 */
	FServerSideRewindResult ConfirmHit(
		const FFramePackage& Package,
		AShooterCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation);
	/**
	 * Projectile
	 */
	FServerSideRewindResult ProjectileConfirmHit(
		const FFramePackage& Package,
		AShooterCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		const float HitTime);
	/**
	 * Shotgun
	 */
	FShotgunServerSideRewindResult ShotgunConfirmHit(
		const TArray<FFramePackage>& FramePackages,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations);
	
private:

	UPROPERTY()
	TObjectPtr<AShooterCharacter> Character;

	UPROPERTY()
	TObjectPtr<AShooterPlayerController> Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;
	UPROPERTY(EditAnywhere)
	float MaxRecordTime;
		
};
