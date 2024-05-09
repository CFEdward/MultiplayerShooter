// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

class AWeapon;
class AShooterPlayerController;

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;

	UPROPERTY()
	bool bHeadshot;
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

	FServerSideRewindResult ServerSideRewind(
		AShooterCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime);

	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(
		AShooterCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime,
		AWeapon* DamageCauser);
	
protected:
	
	virtual void BeginPlay() override;

	void SaveFramePackage();
	void SaveFramePackage(FFramePackage& Package);

	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, const float HitTime);

	FServerSideRewindResult ConfirmHit(
		const FFramePackage& Package,
		AShooterCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation);
	void CacheBoxPositions(AShooterCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveHitBoxes(AShooterCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(AShooterCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(const AShooterCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);

private:

	UPROPERTY()
	TObjectPtr<AShooterCharacter> Character;

	UPROPERTY()
	TObjectPtr<AShooterPlayerController> Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;
	UPROPERTY(EditAnywhere)
	float MaxRecordTime;
		
};
