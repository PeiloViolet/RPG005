// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ARSTypes.h"
#include <GameplayTagContainer.h>
#include "ARSFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDRPGSYSTEM_API UARSFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public: 

	/*Gets all the generations rules related to this Attributes, if PrimaryattributeTag is
	actually a Valid PrimaryAttribute*/
	UFUNCTION(BlueprintCallable, Category = ARS)
	static FGenerationRule GetGenerationRuleByPrimaryAttributeType(FGameplayTag PrimaryAttributeTag);
	
	/*Gets the root GameplayTag for Attributes, the one specified in RPGSettings*/
	UFUNCTION(BlueprintCallable, Category = ARS)
	static FGameplayTag GetAttributesTagRoot();

	/*Gets the root GameplayTag for Parameters, the one specified in RPGSettings*/
	UFUNCTION(BlueprintCallable, Category = ARS)
	static FGameplayTag GetParametersTagRoot();

	/*Gets the root GameplayTag for Statistics, the one specified in RPGSettings*/
	UFUNCTION(BlueprintCallable, Category = ARS)
	static FGameplayTag GetStatisticsTagRoot();

	/*Returns true if TagToCheck is a ChildTag of StatisticTag Root*/
	UFUNCTION(BlueprintCallable, Category = ARS)
	static bool IsValidStatisticTag(FGameplayTag TagToCheck);

	/*Returns true if TagToCheck is a ChildTag of AttributeTag Root*/
	UFUNCTION(BlueprintCallable, Category = ARS)
	static bool IsValidAttributeTag(FGameplayTag TagToCheck);

	/*Returns true if TagToCheck is a ChildTag of ParameterTag Root*/
	UFUNCTION(BlueprintCallable, Category = ARS)
	static bool IsValidParameterTag( FGameplayTag TagToCheck);

	/*Returns the max available level as  specified in the RPGSettings*/
	UFUNCTION(BlueprintCallable, Category = ARS)
	static uint8 GetMaxLevel();

	/*Normalize the current value of a statistic when the max value changes, so that the actual 
	statistic percentage remains the same*/
	UFUNCTION(BlueprintCallable, Category = ARS)
	static float GetNewCurrentValueForNewMaxValue(float oldCurrentValue, float oldMaxValue, float newMaxValue) 
	{
		float _multiplier = oldCurrentValue / oldMaxValue;
		return newMaxValue * _multiplier;
	}
};
