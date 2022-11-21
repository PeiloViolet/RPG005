// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Game/ACFFunctionLibrary.h"
#include "ACFDeveloperSettings.h"
#include "ACMCollisionManagerComponent.h"
#include "Actors/ACFCharacter.h"
#include "Components//ACFTeamManagerComponent.h"
#include "ACMEffectsDispatcherComponent.h"
#include "Game/ACFDamageCalculation.h"
#include "GameFramework/Character.h"
#include "Game/ACFDamageType.h"
#include "Game/ACFGameMode.h"
#include "Game/ACFGameState.h"
#include "Game/ACFPlayerController.h"
#include "Items/ACFWorldItem.h"
#include "UI/ACFHUD.h"
#include <Components/AudioComponent.h>
#include <Curves/CurveFloat.h>
#include <Engine/DataTable.h>
#include <Engine/LevelStreaming.h>
#include <GameFramework/Actor.h>
#include <GameplayTagContainer.h>
#include <GameplayTagsManager.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <NavigationSystem.h>
#include <Sound/SoundCue.h>
#include "Sound/SoundClass.h"
#include "Components/ACFActionsManagerComponent.h"
#include "Game/ACFTypes.h"
#include "Camera/CameraShakeBase.h"
#include "Components/CapsuleComponent.h"


EACFDirection UACFFunctionLibrary::GetActorsRelativeDirection(const AActor* hitDealer, const AActor* receiver)
{
    if (!hitDealer || !receiver) {
        return EACFDirection::Back;
    }

    const FVector damageDealerRelative = hitDealer->GetActorLocation()
        - receiver->GetActorLocation();

    const FVector hitDir = damageDealerRelative.GetUnsafeNormal();

    const float forwardDot = FVector::DotProduct(receiver->GetActorForwardVector(), hitDir);
    const float rightDot = FVector::DotProduct(receiver->GetActorRightVector(), hitDir);
    return GetDirectionFromInput(forwardDot, rightDot);

}

EACFDirection UACFFunctionLibrary::GetDirectionFromInput(const float forwardDot, const float rightDot)
{
	if (forwardDot >= 0.707f)
		return EACFDirection::Front;
	else if (rightDot >= 0.707f)
		return EACFDirection::Right;
	else if (rightDot <= -0.707f)
		return EACFDirection::Left;
	else
		return EACFDirection::Back;

    return EACFDirection::Front;
}

EACFDirection UACFFunctionLibrary::GetDirectionFromAngle(const float angle)
{
    if (angle >= -45 && angle <= 45) {
		return EACFDirection::Front;
    }
    else if (angle >= 45 && angle <= 135) {
		return EACFDirection::Right;
    }
    else if (angle >= -135 && angle <= -45) {
		return EACFDirection::Left;
    }
    else if(angle >= 135 || angle <= -135) {
		return EACFDirection::Back;
    }

	return EACFDirection::Front;
}

void UACFFunctionLibrary::SetSoundClassVolume(USoundClass* TargetClass, float NewVolume)
{
	if (!TargetClass){
        return;
    }
	TargetClass->Properties.Volume = NewVolume;
}

EACFDirection UACFFunctionLibrary::GetHitDirectionByHitResult(const AActor* hitDealer, const FHitResult& hitResult)
{
    ensure(hitDealer);

    AActor* receiver = hitResult.GetActor();

    return GetActorsRelativeDirection(hitDealer, receiver);
}

FVector UACFFunctionLibrary::GetActorsRelativeDirectionVector(const FACFDamageEvent& damageEvent)
{
    if (damageEvent.DamageDealer && damageEvent.DamageReceiver) {
        const FVector damageDealerRelative = damageEvent.DamageReceiver->GetActorLocation() - damageEvent.DamageDealer->GetActorLocation();

        return damageDealerRelative.GetUnsafeNormal();
    }

    return FVector();
}

bool UACFFunctionLibrary::DoesDamageInvolveLocalPlayer(const FACFDamageEvent& damageEvent)
{
    if (!damageEvent.DamageDealer || !damageEvent.DamageReceiver) {
        return false;
    }
    AACFCharacter* localChar = GetLocalACFPlayerCharacter(damageEvent.DamageDealer);
    return damageEvent.DamageDealer == localChar || damageEvent.DamageReceiver == localChar;
}

FVector UACFFunctionLibrary::GetActorsOppositeRelativeDirection(const FACFDamageEvent& damageEvent)
{
    return -GetActorsRelativeDirectionVector(damageEvent);
}

FGameplayTag UACFFunctionLibrary::GetDefaultActionsState()
{
    UACFDeveloperSettings* settings = GetMutableDefault<UACFDeveloperSettings>();

    if (settings) {
        return settings->DefaultActionsState;
    }

    return FGameplayTag();
}

FGameplayTag UACFFunctionLibrary::GetDefaultDeathState()
{
    UACFDeveloperSettings* settings = GetMutableDefault<UACFDeveloperSettings>();

    if (settings) {
        return settings->DefaultDeathState;
    }

    return FGameplayTag();
}

FGameplayTag UACFFunctionLibrary::GetDefaultHitState()
{
    UACFDeveloperSettings* settings = GetMutableDefault<UACFDeveloperSettings>();

    if (settings) {
        return settings->DefaultHitState;
    }

    return FGameplayTag();
}

FGameplayTag UACFFunctionLibrary::GetHealthTag()
{
    UACFDeveloperSettings* settings = GetMutableDefault<UACFDeveloperSettings>();

    if (settings) {
        return settings->HealthTag;
    }

    return FGameplayTag();
}


float UACFFunctionLibrary::ReduceDamageByPercentage(float Damage, float Percentage)
{
    const float _deviation = Damage * Percentage / 100;
    return FMath::Clamp(Damage - _deviation, 0.f, BIG_NUMBER);
}



FGameplayTag UACFFunctionLibrary::GetMovesetTypeTagRoot()
{
    UACFDeveloperSettings* settings = GetMutableDefault<UACFDeveloperSettings>();

    if (settings) {
        return settings->MovesetTypeTag;
    }
    UE_LOG(LogTemp, Warning, TEXT("Missing Moveset Root Tag! - UACFFunctionLibrary "));

    return FGameplayTag();
}

FGameplayTag UACFFunctionLibrary::GetAnimationOverlayTagRoot()
{
    UACFDeveloperSettings* settings = GetMutableDefault<UACFDeveloperSettings>();

    if (settings)
    {
        return settings->AnimationOverlayTag;
    }
    UE_LOG(LogTemp, Warning, TEXT("Missing Animation Overlay Root Tag! - UACFFunctionLibrary "));

    return FGameplayTag();
}


bool UACFFunctionLibrary::IsValidMovesetTypeTag(FGameplayTag TagToCheck)
{
    const FGameplayTag root = GetMovesetTypeTagRoot();
    return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}

bool UACFFunctionLibrary::IsValidAnimOverlayTag(FGameplayTag TagToCheck)
{
    const FGameplayTag root = GetAnimationOverlayTagRoot();
    return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}

void UACFFunctionLibrary::PlayImpactEffect(const FImpactEffect& effect, const FVector& impactLocation, class AActor* instigator, const UObject* WorldContextObject)
{

    if (effect.CameraShake && effect.bShouldShakeCamera && WorldContextObject) {
        UGameplayStatics::PlayWorldCameraShake(WorldContextObject, effect.CameraShake, impactLocation, 0.f, effect.ShakeRadius, effect.ShakeFalloff);
    }

    if (effect.bShouldMakeDamage && instigator) {
        UACMCollisionManagerComponent* collComp = instigator->FindComponentByClass<UACMCollisionManagerComponent>();

        if (!collComp) {
            return;
        }

        collComp->StartTimedSingleTrace(effect.DamageTraceName, effect.DamageTraceDuration);
    }
}

void UACFFunctionLibrary::PlayActionEffect(const FEffect& effect, ACharacter* instigator, const UObject* WorldContextObject)
{
    const AACFGameState* gameState = UACFFunctionLibrary::GetACFGameState(WorldContextObject);

    if (gameState) {
        UACMEffectsDispatcherComponent* EffectComp = gameState->GetEffectsComponent();
        if (EffectComp) {
			EffectComp->ServerPlayReplicatedEffect(effect, instigator);
			return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Missing ACFGameState! - UACFFunctionLibrary "));
}

void UACFFunctionLibrary::PlayActionEffectLocally(const FEffect& effect, class ACharacter* instigator, const UObject* WorldContextObject)
{
    const AACFGameState* gameState = UACFFunctionLibrary::GetACFGameState(WorldContextObject);

    if (gameState) {
        UACMEffectsDispatcherComponent* EffectComp = gameState->GetEffectsComponent();
        EffectComp->PlayEffectLocally(effect, instigator);
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Missing ACFGameState! - UACFFunctionLibrary "));
}

EBattleType UACFFunctionLibrary::GetBattleType(const UObject* WorldContextObject)
{
    const AACFGameState* gameState = Cast<AACFGameState>(UGameplayStatics::GetGameState(WorldContextObject));

    if (gameState) {
        return gameState->GetTeamManager()->GetBattleType();
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Trying to get battle type from client! COnfigurations must be done on server!! - UACFFunctionLibrary "));
    }
    return EBattleType::ETeamBased;
}

TSubclassOf<UACFDamageCalculation> UACFFunctionLibrary::GetDamageCalculationClass()
{
    UACFDeveloperSettings* settings = GetMutableDefault<UACFDeveloperSettings>();

    if (settings) {
        return settings->DamageCalculatorClass;
    }
    UE_LOG(LogTemp, Error, TEXT("Missing DamageCalculator Class! - UACFFunctionLibrary "));

    return nullptr;
}

bool UACFFunctionLibrary::AreEnemyTeams(const UObject* WorldContextObject, ETeam SelfTeam, ETeam TargetTeam)
{
    UACFTeamManagerComponent* teamManager = GetACFTeamManager(WorldContextObject);
    if (teamManager) {
        return teamManager->IsEnemyTeam(SelfTeam, TargetTeam);
    }
    UE_LOG(LogTemp, Error, TEXT("Missing ACFGameState! - UACFFunctionLibrary::AreEnemyTeams "));
    return false;
}

float UACFFunctionLibrary::CalculateDistanceBetweenActors(const AActor* characterA, const AActor* characterB)
{
	const ACharacter* owner = Cast<ACharacter>(characterA);
	const ACharacter* target = Cast<ACharacter>(characterB);

    if (owner && target)
        {
        return CalculateDistanceBetweenCharactersExtents(owner, target);
        }
    return owner->GetDistanceTo(target);
}

float UACFFunctionLibrary::RPGCalculateDistanceBetweenActors(const AActor* characterA, const AActor* characterB, const CalculateDistanceType CalculateDistanceType)
{
    const ACharacter* owner = Cast<ACharacter>(characterA);
    const ACharacter* target = Cast<ACharacter>(characterB);
    
    if (owner && target)
    {
        switch(CalculateDistanceType)
        {
        case CalculateDistanceType::Capsules:
            return CalculateDistanceBetweenCharactersCapsules(owner, target);
        case CalculateDistanceType::Points:
            return CalculateDistanceBetweenCharactersPoints(owner, target);
        case CalculateDistanceType::BoxExtents:
            return CalculateDistanceBetweenCharactersExtents(owner, target);
        }
    }
    return owner->GetDistanceTo(target);
}

float UACFFunctionLibrary::CalculateDistanceBetweenCharactersPoints(const ACharacter* characterA, const ACharacter* characterB)
{
    if (characterA && characterB) {
        return characterA->GetDistanceTo(characterB);
    }
    return -1.f;
}

float UACFFunctionLibrary::CalculateDistanceBetweenCharactersExtents(const ACharacter* characterA, const ACharacter* characterB)
{
    if (characterA && characterB) {
        const FBox aBox = characterA->GetMesh()->Bounds.GetBox();
        const FBox bBox = characterB->GetMesh()->Bounds.GetBox();

        const float dist = characterA->GetDistanceTo(characterB);
        return dist - aBox.GetExtent().X - bBox.GetExtent().X;
    }
    return -1.f;
}

float UACFFunctionLibrary::CalculateDistanceBetweenCharactersCapsules(const ACharacter* characterA, const ACharacter* characterB)
{
    if (characterA && characterB) {
        const float CapsuleRadiusA = characterA->GetCapsuleComponent()->GetScaledCapsuleRadius();
        const float CapsuleRadiusB = characterB->GetCapsuleComponent()->GetScaledCapsuleRadius();
        const float CapsuleHalfHeight_WithoutHemisphereA = characterA->GetCapsuleComponent()->GetScaledCapsuleHalfHeight_WithoutHemisphere();
        const float CapsuleHalfHeight_WithoutHemisphereB = characterB->GetCapsuleComponent()->GetScaledCapsuleHalfHeight_WithoutHemisphere();
        const FVector VectorAB = characterA->GetActorLocation() - characterB->GetActorLocation();
        const FVector UpperSphereCenterLocatiomA = characterA->GetActorLocation() + CapsuleHalfHeight_WithoutHemisphereA;
        const FVector UpperSphereCenterLocatiomB = characterB->GetActorLocation() + CapsuleHalfHeight_WithoutHemisphereA;
        const FVector LowerSphereCenterLocatiomA = characterA->GetActorLocation() - CapsuleHalfHeight_WithoutHemisphereA;
        const FVector LowerSphereCenterLocatiomB = characterB->GetActorLocation() - CapsuleHalfHeight_WithoutHemisphereA;
        const float XYDistance =  UKismetMathLibrary::VSizeXY(VectorAB);
        
        if(XYDistance > CapsuleRadiusA + CapsuleRadiusB)
        {
            //半径和外  不相交
            if(VectorAB.Z < CapsuleHalfHeight_WithoutHemisphereA + CapsuleHalfHeight_WithoutHemisphereB)
            {
                //垂直段
                return XYDistance - CapsuleRadiusA - CapsuleRadiusB;
            }
            else
            {
                //半球段
                return UKismetMathLibrary::FMin(UKismetMathLibrary::Abs((UpperSphereCenterLocatiomA - LowerSphereCenterLocatiomB).Z),
                                                UKismetMathLibrary::Abs((LowerSphereCenterLocatiomA - UpperSphereCenterLocatiomB).Z));
            }
        }
        else
        {
            //半径和内
            if(VectorAB.Z > UKismetMathLibrary::Abs(CapsuleHalfHeight_WithoutHemisphereA + CapsuleHalfHeight_WithoutHemisphereB))
            {
                //高度不相交
                return UKismetMathLibrary::FMin(UKismetMathLibrary::Abs((UpperSphereCenterLocatiomA - LowerSphereCenterLocatiomB).Z),
                                                UKismetMathLibrary::Abs((LowerSphereCenterLocatiomA - UpperSphereCenterLocatiomB).Z));
            }
            else
            {
                //高度可能相交
                if(XYDistance < UKismetMathLibrary::Abs(CapsuleRadiusA - CapsuleRadiusB))
                {
                    //XY包裹段
                    
                    return XYDistance - UKismetMathLibrary::FMax(CapsuleRadiusA, CapsuleRadiusB);
                }
                else
                {
                    return 0.f;
                }
            }
        }
    }
    else
    {
        return -1.f;
    }
}

float UACFFunctionLibrary::CalculateAngleBetweenVectors(const FVector& vectorA, const FVector& vectorB)
{
    return FMath::RadiansToDegrees(acosf(FVector::DotProduct(vectorA, vectorB)));

}
//计算Actor方位角
FRotator UACFFunctionLibrary::CalculateBAZBetweenVectors(const FRotator RotatorA, const FVector& VectorA, const FVector& VectorB)
{
    return UKismetMathLibrary::NormalizedDeltaRotator( UKismetMathLibrary::FindLookAtRotation(VectorA, VectorB), RotatorA);
}

class AACFHUD* UACFFunctionLibrary::GetACFHUD(const UObject* WorldContextObject)
{
    APlayerController* playerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);

    return Cast<AACFHUD>(playerController->GetHUD());
}

class AACFPlayerController* UACFFunctionLibrary::GetLocalACFPlayerController(const UObject* WorldContextObject)
{
    return Cast<AACFPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
}

class AACFGameMode* UACFFunctionLibrary::GetACFGameMode(const UObject* WorldContextObject)
{
    return Cast<AACFGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
}

class AACFGameState* UACFFunctionLibrary::GetACFGameState(const UObject* WorldContextObject)
{
    return Cast<AACFGameState>(UGameplayStatics::GetGameState(WorldContextObject));
}

class AACFCharacter* UACFFunctionLibrary::GetLocalACFPlayerCharacter(const UObject* WorldContextObject)
{
    return Cast<AACFCharacter>(UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0));
}

UACFTeamManagerComponent* UACFFunctionLibrary::GetACFTeamManager(const UObject* WorldContextObject)
{
    AACFGameState* GameState = GetACFGameState(WorldContextObject);
    if (GameState) {
        return GameState->GetTeamManager();
    }
    UE_LOG(LogTemp, Error, TEXT("Missing Game State - UACFFunctionLibrary::GetACFTeamManager"));
    return nullptr;
}

bool UACFFunctionLibrary::ShouldExecuteAction(const FActionStruct& Action,const AACFCharacter* CharacterOwner)
{
	return CharacterOwner && CharacterOwner->GetActionsComponent() &&
		CharacterOwner->GetActionsComponent()->CanExecuteAction(Action.ActionTag) &&
		FMath::RandRange(0.f, 100.f) < Action.ProbabilityProportion && (
			(uint8)CharacterOwner->GetRelativeTargetDirection(CharacterOwner->GetTarget()) == (uint8)Action.AllowedFromDirection ||
			Action.AllowedFromDirection == EActionDirection::EveryDirection
			);
}

