// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "ACFDamageType.h"
#include "ACFDamageCalculation.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class ASCENTCOMBATFRAMEWORK_API UACFDamageCalculation : public UObject
{
	GENERATED_BODY()

protected: 

	/*For every damagetype, the parameters that will be used to calculate the damage and a scaling factor
	Total Damage will be the sum of all ATTACK DAMAGE INFLUENCES of the DAMAGE DEALER  * scalingfactor */
	/*DEFENSE PARAMETER PERCENTAGES are the parameters that will be used from the DAMAGE RECEIVER to 
	REDUCE the incoming damage and a scaling factor.
	Total Damage will be reduced by the sum of all the influences * scalingfactor,  in percentages.
	if the sum of all those parameters scaled is 30, incoming damage will be reduced by 30%.
	If this number is >= 100, damage will be reduced to 0*/
	UPROPERTY(EditDefaultsOnly, Category = ACF)
	TMap<TSubclassOf<UDamageType>, FDamageInfluences> DamageInfluencesByParameter;

	/*For every hitresponse you can define a multiplier to be applied to the final damage*/
	UPROPERTY(EditDefaultsOnly, Category = ACF)
	TMap <FGameplayTag, float> HitResponseActionMultiplier;

	/*For every damagetype, the parameter to be used to calculate crit chance. 100 means always crit*/
	UPROPERTY(EditDefaultsOnly, Category = ACF)
	TMap <TSubclassOf<UDamageType>, FDamageInfluence> CritChancePercentageByParameter;

	

	/*Multiplier applied when an hit is Critical*/
	UPROPERTY(EditDefaultsOnly, Category = ACF)
	float critMultiplier = 1.5f;

	/*Random deviation in percentage added to final damage. % means that final damage will be randomized
	in the range -5% , +5%*/
	UPROPERTY(EditAnywhere, Category = ACF)
	float DefaultRandomDamageDeviationPercentage = 5.0f;

	/*Multiplier applied depending on the damaged zone of the receiver*/
	UPROPERTY(EditAnywhere, Category = ACF)
	TMap<EDamageZone, float> DamageZoneToDamageMultiplier;

	/*The parameter to be used to reduce incoming damage when in Defense State*/
	UPROPERTY(EditDefaultsOnly, Category = ACF)
	FGameplayTag DefenseStanceParameterWhenBlocked;


	/*This statistic is used to prevent the owner to go on Hit Action ( = being staggered).
	If set, this statistic will be reduced by the actual damage at every hit and the owner
	will only be staggered when this statistic reaches 0. (make sure regen is activate, otherwise
	the AI after the first stagger, will be always be staggered) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	FGameplayTag StaggerResistanceStastistic;
public: 

	
	UFUNCTION(BlueprintNativeEvent, Category = ACF)
		FACFDamageEvent CalculateFinalDamage(const FACFDamageEvent& inDamageEvent);
	virtual FACFDamageEvent CalculateFinalDamage_Implementation(const FACFDamageEvent& inDamageEvent);

	UFUNCTION(BlueprintNativeEvent, Category = ACF)
		void PreDamageCalculation(const FACFDamageEvent& inDamageEvent);
	virtual void PreDamageCalculation_Implementation(const FACFDamageEvent& inDamageEvent);

	UFUNCTION(BlueprintNativeEvent, Category = ACF)
		void PostDamageCalculation(const FACFDamageEvent& inDamageEvent, FACFDamageEvent& finalDamageEvent);
	virtual void PostDamageCalculation_Implementation(const FACFDamageEvent& inDamageEvent, FACFDamageEvent& finalDamageEvent);
};
