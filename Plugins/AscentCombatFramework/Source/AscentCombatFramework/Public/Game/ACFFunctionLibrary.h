// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "ACFDamageType.h"
#include "ACFTypes.h"
#include "ARSTypes.h"
#include "CoreMinimal.h"
#include "Items/ACFItem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Engine/DataTable.h>
#include <GameplayTagContainer.h>
#include "ACFFunctionLibrary.generated.h"

UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFFunctionLibrary : public UBlueprintFunctionLibrary {
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static EACFDirection GetHitDirectionByHitResult(const AActor* hitDealer, const FHitResult& hitResult);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static FVector GetActorsRelativeDirectionVector(const FACFDamageEvent& damageEvent);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static bool DoesDamageInvolveLocalPlayer(const FACFDamageEvent& damageEvent);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static EACFDirection GetActorsRelativeDirection(const AActor* hitDealer, const AActor* receiver);

	UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static EACFDirection GetDirectionFromInput(const float forwardDot, const float rightDot);

	UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static EACFDirection GetDirectionFromAngle(const float angle);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static void SetSoundClassVolume(class USoundClass* TargetClass, float NewVolume);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static FVector GetActorsOppositeRelativeDirection(const FACFDamageEvent& damageEvent);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static FGameplayTag GetDefaultActionsState();

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static FGameplayTag GetDefaultDeathState();

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static FGameplayTag GetDefaultHitState();

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static FGameplayTag GetHealthTag();

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static float ReduceDamageByPercentage(float Damage, float Percentage);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static FGameplayTag GetMovesetTypeTagRoot();

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static FGameplayTag GetAnimationOverlayTagRoot();

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static bool IsValidMovesetTypeTag(FGameplayTag TagToCheck);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static bool IsValidAnimOverlayTag(FGameplayTag TagToCheck);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = ACFLibrary)
    static void PlayImpactEffect(const FImpactEffect& effect, const FVector& impactLocation, class AActor* instigator, const UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = ACFLibrary)
    static void PlayActionEffect(const FEffect& effect, class ACharacter* instigator, const UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = ACFLibrary)
    static void PlayActionEffectLocally(const FEffect& effect, class ACharacter* instigator, const UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = ACFLibrary)
    static EBattleType GetBattleType(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = ACFLibrary)
    static TSubclassOf<class UACFDamageCalculation> GetDamageCalculationClass();

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static bool AreEnemyTeams(const UObject* WorldContextObject, ETeam teamA, ETeam teamB);

	UFUNCTION(BlueprintCallable, Category = ACFLibrary)
	static float CalculateDistanceBetweenActors(const AActor* characterA, const AActor* characterB);
	
	UFUNCTION(BlueprintCallable, Category = RPGLibrary)
	static float RPGCalculateDistanceBetweenActors(const AActor* characterA, const AActor* characterB, const CalculateDistanceType CalculateDistanceType);

	UFUNCTION(BlueprintCallable, Category = ACFLibrary)
	static float CalculateDistanceBetweenCharactersPoints(const ACharacter* characterA, const ACharacter* characterB);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static float CalculateDistanceBetweenCharactersExtents(const ACharacter* characterA, const ACharacter* characterB);

	UFUNCTION(BlueprintCallable, Category = RPGLibrary)
    static float CalculateDistanceBetweenCharactersCapsules(const ACharacter* characterA, const ACharacter* characterB);

    UFUNCTION(BlueprintCallable, Category = ACFLibrary)
    static float CalculateAngleBetweenVectors(const FVector& vectorA, const FVector& vectorB);

	UFUNCTION(BlueprintCallable, Category = ACFLibrary)
	static FRotator CalculateBAZBetweenVectors(const FRotator RotatorA, const FVector& VectorA, const FVector& VectorB);
	
    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = ACFLibrary)
    static class AACFHUD* GetACFHUD(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = ACFLibrary)
    static class AACFPlayerController* GetLocalACFPlayerController(const UObject* WorldContextObject);

    /*Works on server only*/
    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = ACFLibrary)
    static class AACFGameMode* GetACFGameMode(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = ACFLibrary)
    static class AACFGameState* GetACFGameState(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = ACFLibrary)
    static class AACFCharacter* GetLocalACFPlayerCharacter(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = ACFLibrary)
	static class UACFTeamManagerComponent* GetACFTeamManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = ACFLibrary)
	static bool ShouldExecuteAction(const FActionStruct& Action, const AACFCharacter* CharacterOwner);

    template <typename TEnum>
    static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
    {
        const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
        if (!enumPtr) {
            return FString("Invalid");
        }

        return enumPtr->GetNameByValue((int64)Value).ToString();
    }

};
