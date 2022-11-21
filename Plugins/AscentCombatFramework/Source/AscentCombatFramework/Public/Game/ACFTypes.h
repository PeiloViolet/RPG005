// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include <Engine/DataTable.h>
#include "ARSTypes.h"
#include <GameFramework/DamageType.h>
#include "GenericTeamAgentInterface.h"
#include "Templates/SubclassOf.h"
#include "NiagaraSystem.h"
#include "Camera/CameraShakeBase.h"
#include "Interfaces/ACFEntityInterface.h"
#include <GameplayTagContainer.h>
#include "ACMTypes.h"
#include "ACFTypes.generated.h"

/**
 *
 */
 
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFTypes : public UObject
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class ELocomotionState : uint8
{
	EIdle = 0       UMETA(DisplayName = "Idle"),
	EWalk			UMETA(DisplayName = "Walk"),
	EJog			UMETA(DisplayName = "Jog"),
	ESprint			UMETA(DisplayName = "Sprint"),
};

UENUM(BlueprintType)
enum class EDeathType : uint8
{
	EGoRagdoll = 0       UMETA(DisplayName = "Go Ragdoll"),
	EDeathAction         UMETA(DisplayName = "Trigger Death Action"),
	EDoNothing           UMETA(DisplayName = "Do Nothing"),
};

USTRUCT(BlueprintType)
struct FDamageInfluence
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	FGameplayTag Parameter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	float ScalingFactor = 1.f;

};

USTRUCT(BlueprintType)
struct FDamageInfluences
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	TArray<FDamageInfluence> AttackParametersInfluence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	TArray<FDamageInfluence> DefenseParametersPercentages;

};

USTRUCT(BlueprintType)
struct FActionState : public FACFStruct
{
	GENERATED_BODY()

public:

	FActionState() {
		MontageAction = nullptr;
		Action = nullptr;
	}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	class UAnimMontage* MontageAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = ACF)
	class UACFBaseAction* Action;

};

USTRUCT(BlueprintType)
struct FRagdollImpulse 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	float ImpulseIntensity = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	FVector ImpulseDirection = FVector(0.f, 0.f, -1.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	FName BoneName = "pelvis";

	FRagdollImpulse(){};

};

UENUM(BlueprintType)
enum class EDamageActivationType : uint8
{
	ERight        UMETA(DisplayName = "Right Weapon Only"),
	ELeft		  UMETA(DisplayName = "Left Weapon Only"),
	EBoth		  UMETA(DisplayName = "Left And Right Weapon"),
	EPhysical      UMETA(DisplayName = "Physical Damage"),
};

UENUM(BlueprintType)
enum class EBattleState : uint8
{
	EExploration        UMETA(DisplayName = "Exploration"),
	EBattle		 UMETA(DisplayName = "Battle"),
};

UENUM(BlueprintType)
enum class EDamageZone : uint8
{
	ENormal         UMETA(DisplayName = "Normal Zone"),
	EImmune		 UMETA(DisplayName = "Zero Damage Zone"),
	EHighDefense	 UMETA(DisplayName = "High Defense Zone"),
	EHighDamage	 UMETA(DisplayName = "High Damage Zone"),
};

UENUM(BlueprintType)
enum class EACFDirection : uint8
{
	Front  = 0,
	Back = 1,
	Left = 2,
	Right = 3
};

UENUM(BlueprintType)
enum class EActionDirection : uint8
{
	Front  = 0			UMETA(DisplayName = "Front Only"),
	Back  = 1			UMETA(DisplayName = "Back Only"), 
	Left = 2			UMETA(DisplayName = "Left Only"),
	Right  = 3			 UMETA(DisplayName = "Right Only"),
	EveryDirection = 4 UMETA(DisplayName = "Every Direction"),
};

UENUM(BlueprintType)
enum class EMontageReproductionType : uint8
{
	ERootMotion							UMETA(DisplayName = "Root Motion"),
	ERootMotionScaled					UMETA(DisplayName = "Root Motion Scaled"),
	ECurveOverrideSpeed 				UMETA(DisplayName = "Override Speed with Curve"),
	ECurveSnapsOnTarget 				UMETA(DisplayName = "Override Speed and Snaps To Target"),
	ECurveOverrideSpeedAndDirection 	UMETA(DisplayName = "Override Speed and Direction Vector"),
};

UENUM(BlueprintType)
enum class EActionPriority : uint8
{
	ENone   UMETA(DisplayName = "Very Low"),
	ELow    UMETA(DisplayName = "Low"),
	EMedium UMETA(DisplayName = "Medium"),
	EHigh   UMETA(DisplayName = "High"),
	EHighest  UMETA(DisplayName = "Highest"),
};

UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	EActive  UMETA(DisplayName = "Active: Activate Once and Destroy"),
	EPassive  UMETA(DisplayName = "Passive: Stay Activated"),
};

USTRUCT(BlueprintType)
struct FSnapConfiguration
{
	GENERATED_BODY()

public:
	FSnapConfiguration():target(nullptr){};
	UPROPERTY( BlueprintReadWrite, Category = ACF)
	AActor* target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	float preferredDistance = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	float maxSnapMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	float RotationRateSnapMult = 1.0f;

};

USTRUCT(BlueprintType)
struct FImpactEffect
{
	GENERATED_BODY()

public:
	FImpactEffect():bShouldShakeCamera(true),bShouldMakeDamage(true),DamageTraceDuration(0.f){};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	bool bShouldShakeCamera;

	UPROPERTY(EditAnywhere, meta = (EditCondition = bShouldShakeCamera), BlueprintReadWrite, Category = ACF)
	TSubclassOf<class UCameraShakeBase> CameraShake;

	UPROPERTY(EditAnywhere, meta = (EditCondition = bShouldShakeCamera), BlueprintReadWrite, Category = ACF)
	float ShakeFalloff = 100.f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = bShouldShakeCamera), BlueprintReadWrite, Category = ACF)
	float ShakeRadius = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	bool bShouldMakeDamage;

	UPROPERTY(EditAnywhere, meta = (EditCondition = bShouldMakeDamage), BlueprintReadWrite, Category = ACF)
	FName DamageTraceName;

	UPROPERTY(EditAnywhere, meta = (EditCondition = bShouldMakeDamage), BlueprintReadWrite, Category = ACF)
	float DamageTraceDuration;

};

USTRUCT(BlueprintType)
struct FActionConfig
{
	GENERATED_BODY()

public:
	FActionConfig():CoolDownTime(0.5){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFStatConfig)
	TArray<FStatisticModifier> ActionCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFStatConfig)
	TArray<FAttribute> Requirements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFStatConfig)
	FAttributesSetModifier AttributeModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFActionConfig)
	FEffect ActionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFActionConfig)
	bool bPerformableInAir = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFActionConfig)
	bool bCanBeStored = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFActionConfig)
	float CoolDownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFAnimConfig)
	bool bPlayRandomMontageSection = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFAnimConfig)
	bool bBindActionToAnimation = true;

	/*Decide if we have to stop AIController's BT during this action*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFAnimConfig)
	bool bStopBehavioralThree = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFAnimConfig)
	bool bPlayEffectOnActionStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACFAnimConfig)
	EMontageReproductionType MontageReproductionType = EMontageReproductionType::ERootMotion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "MontageReproductionType == EMontageReproductionType::ERootMotionScaled"),  Category = ACFAnimConfig)
	float RootMotionScale = 1.f;

};

USTRUCT(BlueprintType)
struct FActionStruct : public FTableRowBase
{
	GENERATED_BODY()
	bool operator == (const FActionStruct& ActionChances) const
	{
		if(this->AllowedFromDirection != ActionChances.AllowedFromDirection) return false;
		if(this->ActionTag != ActionChances.ActionTag) return false;
		if(this->UseCondition != ActionChances.UseCondition) return false;
		if(this->Condition != ActionChances.Condition) return false;
		if(this->ProbabilityProportion != ActionChances.ProbabilityProportion) return false;
		if(this->ProbabilityProportionweight != ActionChances.ProbabilityProportionweight) return false;
		if(this->BTWaitTime != ActionChances.BTWaitTime) return false;
		if(this->Priority != ActionChances.Priority) return false;
		if(this->ActionTag != ActionChances.ActionTag) return false;
		return true;
	};

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RPG)
	EActionDirection AllowedFromDirection = EActionDirection::Front;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RPG)
	FGameplayTag ActionTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RPG)
	bool UseCondition {false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "UseCondition"),Instanced, Category = RPG)
	class UACFActionCondition* Condition = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RPG)
	float ProbabilityProportion = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RPG)
	float ProbabilityProportionweight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RPG)
	float BTWaitTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RPG)
	EActionPriority Priority = EActionPriority::ELow ;
};

USTRUCT(BlueprintType)
struct FActionsGroupStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	TArray< FActionStruct> ActionsGroup;
};

/*
USTRUCT(BlueprintType)
struct FConditions : public FActionStruct
{
	GENERATED_BODY()
	FConditions()
	{
		UseCondition = true;
	}

};

USTRUCT(BlueprintType)
struct FConditionsActionChances 
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	TArray< FConditions> ActionsGroup;
};
*/

USTRUCT(BlueprintType)
struct FOnHitActionChances : public FActionStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	TArray<TSubclassOf<UDamageType>> AllowedDamageTypes;
};

USTRUCT(BlueprintType)
struct FEffectByDamageType : public FACFStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	TMap<TSubclassOf<UDamageType>, FEffect> EffectByDamageType;

};

UENUM(BlueprintType)
enum class ECombatType : uint8
{
	EUnarmed   UMETA(DisplayName = "Unarmed"),
	EMelee	    UMETA(DisplayName = "Melee"),
	ERanged    UMETA(DisplayName = "Ranged"),
};

UENUM(BlueprintType)
enum class ECombatBehaviorType : uint8
{
	EMelee	    UMETA(DisplayName = "Melee"),
	ERanged     UMETA(DisplayName = "Ranged"),
};

UENUM(BlueprintType)
enum class CalculateDistanceType : uint8
{
	Capsules  = 0	 UMETA(DisplayName = "Capsules"),
	Points           UMETA(DisplayName = "Points"),
	BoxExtents	     UMETA(DisplayName = "BoxExtents"),
};


