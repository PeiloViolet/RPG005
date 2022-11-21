// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACFActionCondition.h"
#include "Actors/ACFCharacter.h"
#include "ACFAIController.h"
#include "ARSFunctionLibrary.h"
#include "ARSStatisticsComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include "BlueprintGameplayTagLibrary.h"
#include "Kismet/KismetMathLibrary.h"


bool UACFDistanceActionCondition::IsConditionMet_Implementation(const AACFCharacter* character)
  {
	if (!character)
		return false;

	const AACFCharacter* target = Cast<AACFCharacter>(character->GetTarget());

	if ( !target)
		return false;

	switch (ConditionType)
	{
	case EConditionType::EAbove:
		return UACFFunctionLibrary::RPGCalculateDistanceBetweenActors(character, target ,CalculateDistanceType) > Distance;
	case EConditionType::EBelow:
		return UACFFunctionLibrary::RPGCalculateDistanceBetweenActors(character, target ,CalculateDistanceType) < Distance;
	case EConditionType::EEqual:
		return (UACFFunctionLibrary::RPGCalculateDistanceBetweenActors(character, target ,CalculateDistanceType) > Distance - NearlyEqualAcceptance &&
			UACFFunctionLibrary::RPGCalculateDistanceBetweenActors(character, target ,CalculateDistanceType) < Distance + NearlyEqualAcceptance);
	}
	return false;
}

bool UACFStatisticActionCondition::IsConditionMet_Implementation(const AACFCharacter* character)
{
	if (!character)
		return false;

	if (!UARSFunctionLibrary::IsValidStatisticTag(StatisticTag))
		return false;

	const float statValue = character->GetStatisticsComponent()->GetCurrentValueForStatitstic(StatisticTag);

	switch (ConditionType)
	{
	case EConditionType::EAbove:
		return statValue > StatisticValue;
		break;
	case EConditionType::EBelow:
		return statValue < StatisticValue;
		break;
	case EConditionType::EEqual:
		return (statValue > StatisticValue - NearlyEqualAcceptance ||
			statValue < StatisticValue + NearlyEqualAcceptance);
		break;
	}

	return false;
}

bool URPGAllowAngleActionCondition::IsConditionMet_Implementation(const AACFCharacter* character)
{
	if (!character) return false;

	const AACFCharacter* target = Cast<AACFCharacter>(character->GetTarget());

	if (!target) return false;

	AngleValue = UACFFunctionLibrary::CalculateBAZBetweenVectors(character->GetActorRotation(),character->GetActorLocation(),target->GetActorLocation());

	if(UKismetMathLibrary::Abs(UKismetMathLibrary::NormalizedDeltaRotator(AngleValue, AllowableAngleRange.RangeOrientationAngle).Yaw) <
	   UKismetMathLibrary::Abs(AllowableAngleRange.FanOpeningAngle.Yaw/2.f))
	{
		return true;
	}
	return false;
}

bool URPGAllowTargetActionCondition::IsConditionMet_Implementation(const AACFCharacter* character)
{
	if (!character) return false;
	const AACFCharacter* TargetActor = Cast<AACFCharacter>(character->GetTarget());

	if (!TargetActor) return false;

	if(UBlueprintGameplayTagLibrary::MatchesTag(TargetActor->GetCurrentActionState(), AllowTargetAction, false))
	{
		return true;
	}
	return false;
}
