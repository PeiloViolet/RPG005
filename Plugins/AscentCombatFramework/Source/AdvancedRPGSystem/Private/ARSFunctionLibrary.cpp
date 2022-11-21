// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "ARSFunctionLibrary.h"
#include "ARSDeveloperSettings.h"
#include "ARSTypes.h"
#include <GameplayTagsManager.h>





FGenerationRule UARSFunctionLibrary::GetGenerationRuleByPrimaryAttributeType(FGameplayTag PrimaryAttributeTag)
{
	UARSDeveloperSettings* _settings = GetMutableDefault<UARSDeveloperSettings>();

	if (_settings)
	{
		FGenerationRule* _rule = _settings->SecondaryAttGenerationRules.Find(PrimaryAttributeTag);
		UE_LOG(LogTemp, Warning, TEXT("TRUE"));
		if (_rule)
		{
			return *(_rule);
		}
	}	
	UE_LOG(LogTemp, Warning, TEXT("Missing Generation Rule! - ARSFUNCTION LIBRARY"));	
	return FGenerationRule();	
}

FGameplayTag UARSFunctionLibrary::GetAttributesTagRoot()
{
	UARSDeveloperSettings* _settings = GetMutableDefault<UARSDeveloperSettings>();

	if (_settings)
	{
		return _settings->AttributesRoot;
	}
	UE_LOG(LogTemp, Warning, TEXT("Missing Tag! - ARSFUNCTION LIBRARY"));
	return FGameplayTag();
}

FGameplayTag UARSFunctionLibrary::GetParametersTagRoot()
{
	UARSDeveloperSettings* _settings = GetMutableDefault<UARSDeveloperSettings>();

	if (_settings)
	{
		return _settings->ParametersRoot;
	}
	UE_LOG(LogTemp, Warning, TEXT("Missing Tag! - ARSFUNCTION LIBRARY"));
	return FGameplayTag();
}

FGameplayTag UARSFunctionLibrary::GetStatisticsTagRoot()
{
	UARSDeveloperSettings* _settings = GetMutableDefault<UARSDeveloperSettings>();

	if (_settings)
	{
		return _settings->StatisticsRoot;
	}
	UE_LOG(LogTemp, Warning, TEXT("Missing Tag! - ARSFUNCTION LIBRARY"));
	return FGameplayTag();
}

uint8 UARSFunctionLibrary::GetMaxLevel()
{
	UARSDeveloperSettings* _settings = GetMutableDefault< UARSDeveloperSettings>();

	if (_settings)
	{
		return _settings->MaxLevel;
	}
	UE_LOG(LogTemp, Warning, TEXT("Missing MaxLevel! - ARSFUNCTION LIBRARY"));

	return 0;
}

bool UARSFunctionLibrary::IsValidStatisticTag(FGameplayTag TagToCheck)
{
	FGameplayTag root = GetStatisticsTagRoot();

	return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}

bool UARSFunctionLibrary::IsValidAttributeTag(FGameplayTag TagToCheck)
{
	FGameplayTag root = GetAttributesTagRoot();

	return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}

bool UARSFunctionLibrary::IsValidParameterTag(FGameplayTag TagToCheck)
{
	FGameplayTag root = GetParametersTagRoot();

	return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}
