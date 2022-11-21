// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once


#include "CoreMinimal.h"
#include <GameFramework/DamageType.h>
#include "Templates/SubclassOf.h"
#include "ACMTypes.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FHitActors {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = ACM)
    TArray<class AActor*> AlreadyHitActors;
};

UENUM(BlueprintType)
enum class EDebugType : uint8 {
    EDontShowDebugInfos UMETA(DisplayName = "Don't Show Debug Info"),
    EShowInfoDuringSwing UMETA(DisplayName = "Show Info During Swing"),
    EAlwaysShowDebug UMETA(DisplayName = "Always Show Debug Info"),
};

UENUM(BlueprintType)
enum class EDamageType : uint8 {
    EPoint UMETA(DisplayName = "Point Damage"),
    EArea UMETA(DisplayName = "Area Damage"),
};

UENUM(BlueprintType)
enum class ESpawnFXLocation : uint8
{
	ESpawnOnActorLocation  UMETA(DisplayName = "Attached to Actor"),
	ESpawnAttachedToSocketOrBone UMETA(DisplayName = "Attached to Socket / Bone"),
	ESpawnAtLocation    UMETA(DisplayName = "Spawn On Location")
};

USTRUCT(BlueprintType)
struct FEffect
{
	GENERATED_BODY()

public:
	FEffect(): ActionSound(nullptr), ActionParticle(nullptr), NiagaraParticle(nullptr){}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
		ESpawnFXLocation SpawnLocation = ESpawnFXLocation::ESpawnOnActorLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
		class USoundCue* ActionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
		FName SocketOrBoneName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
		class UParticleSystem* ActionParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
		class UNiagaraSystem* NiagaraParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
		float NoiseEmitted = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
		FTransform RelativeOffset;

};

USTRUCT(BlueprintType)
struct FBaseTraceInfo {

    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACM)
    FName StartSocket;

    /** The type of damage applied*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = bAutoApplyDamage), Category = ACM)
    TSubclassOf<class UDamageType> DamageTypeClass =  UDamageType::StaticClass();

    /** The base damage to apply to the actor (Can be modified in you TakeDamage Implementation)*/
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = bAutoApplyDamage), Category = ACM)
    float BaseDamage = 0.f;
};

USTRUCT(BlueprintType)
struct FLineTraceInfo : public FBaseTraceInfo {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACM)
    float LineTraceLength = 3500.f;
};

USTRUCT(BlueprintType)
struct FTraceInfo : public FBaseTraceInfo {
    GENERATED_BODY()

public:
	FTraceInfo(): Radius(0.f),DamageType(EDamageType::EArea), AttackSound(nullptr), AttackParticle(nullptr){}

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACM)
    float Radius;

    /** Select if it's Area or Point Damage*/
    UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (EditCondition = bAutoApplyDamage), Category = ACM)
    EDamageType DamageType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACM)
    float TrailLength = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACM)
	class USoundCue* AttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACM)
	class UParticleSystem* AttackParticle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ACM)
	FName EndSocket;
};

UCLASS()
class COLLISIONSMANAGER_API UACMTypes : public UObject {
    GENERATED_BODY()
};
