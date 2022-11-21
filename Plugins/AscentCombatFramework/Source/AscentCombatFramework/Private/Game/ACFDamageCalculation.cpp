// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Game/ACFDamageCalculation.h"
#include "ARSStatisticsComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include "Components/ACFDefenseStanceComponent.h"


FACFDamageEvent UACFDamageCalculation::CalculateFinalDamage_Implementation(const FACFDamageEvent& inDamageEvent)
{
	PreDamageCalculation(inDamageEvent);
	FACFDamageEvent outDamage = inDamageEvent;

	if (!inDamageEvent.DamageDealer || !inDamageEvent.DamageReceiver) {
		UE_LOG(LogTemp, Error, TEXT("Missing Damage Dealer or receiver!!! - UACFDamageCalculation::CalculateFinalDamage"));
		return outDamage;
	}

	if (!inDamageEvent.DamageClass) {
		UE_LOG(LogTemp, Error, TEXT("Missing Damage Class!!!! - UACFDamageCalculation::CalculateFinalDamage"));
		return outDamage;
	}

	const FDamageInfluences* damagesInf = DamageInfluencesByParameter.Find(inDamageEvent.DamageClass);
	if (!damagesInf) {
		UE_LOG(LogTemp, Error, TEXT("DamageClass Influence NOT Set!!!! - UACFDamageCalculation::CalculateFinalDamage"));
		return outDamage;
	}

	float totalDamage = inDamageEvent.FinalDamage;

	const UARSStatisticsComponent* dealerComp = inDamageEvent.DamageDealer->FindComponentByClass<UARSStatisticsComponent>();
	UARSStatisticsComponent* receiverComp = inDamageEvent.DamageReceiver->FindComponentByClass<UARSStatisticsComponent>();


	//First we calculate the sum of every parameter influence
	for (const auto damInf : damagesInf->AttackParametersInfluence) {
		totalDamage += dealerComp->GetParameterValue(damInf.Parameter) * damInf.ScalingFactor;
	}

	//Then reduces it for defenses
	const FDamageInfluences* defenseInf = DamageInfluencesByParameter.Find(inDamageEvent.DamageClass);
	if (defenseInf) {
		for (const auto damInf : defenseInf->DefenseParametersPercentages) {
			totalDamage = UACFFunctionLibrary::ReduceDamageByPercentage(totalDamage,
				receiverComp->GetParameterValue(damInf.Parameter) * damInf.ScalingFactor);
		}
	}

	outDamage.bIsCritical = false;
	//Then apply crit
	const FDamageInfluence* critChance = CritChancePercentageByParameter.Find(inDamageEvent.DamageClass);
	if (critChance) {
		const float percentage = dealerComp->GetParameterValue(critChance->Parameter) * critChance->ScalingFactor;
		if (FMath::RandRange(0.f, 100.f) < percentage) {
			totalDamage *= critMultiplier;
			outDamage.bIsCritical = true;
		}
	}

	//Final Randomization
	if (totalDamage != 0.f) {
		const float deviation = totalDamage * DefaultRandomDamageDeviationPercentage / 100;
		totalDamage = FMath::FRandRange(totalDamage - deviation, totalDamage + deviation);
	}

	outDamage.FinalDamage = totalDamage;
	UACFDefenseStanceComponent* defComp = inDamageEvent.DamageReceiver->FindComponentByClass<UACFDefenseStanceComponent>();
	if (defComp && defComp->IsInDefensePosition() && defComp->TryBlockIncomingDamage(outDamage, outDamage.HitResponseAction)) {
		const float reducedPercentage = receiverComp->GetParameterValue(DefenseStanceParameterWhenBlocked);
		totalDamage = UACFFunctionLibrary::ReduceDamageByPercentage(totalDamage, reducedPercentage);
	}
	else {
		//Damage Zones
		const float* zoneMult = DamageZoneToDamageMultiplier.Find(inDamageEvent.DamageZone);
		if (zoneMult) {
			totalDamage *= *zoneMult;
		}

		//Hit Responses
		const float* hitMult = HitResponseActionMultiplier.Find(inDamageEvent.HitResponseAction);
		if (hitMult) {
			totalDamage *= *hitMult;
		}
	}
	
	if (StaggerResistanceStastistic != FGameplayTag() && outDamage.HitResponseAction == UACFFunctionLibrary::GetDefaultHitState()) {
		receiverComp->ModifyStatistic(StaggerResistanceStastistic, -totalDamage);
		if (receiverComp->GetCurrentValueForStatitstic(StaggerResistanceStastistic) > 1.f) {
			outDamage.HitResponseAction = FGameplayTag();
		}
	}
	outDamage.FinalDamage = totalDamage;

	FACFDamageEvent FinalDamage;
	PostDamageCalculation(outDamage, FinalDamage);
	return FinalDamage;
}

void UACFDamageCalculation::PreDamageCalculation_Implementation(const FACFDamageEvent& inDamageEvent)
{

}

void UACFDamageCalculation::PostDamageCalculation_Implementation(const FACFDamageEvent& inDamageEvent, FACFDamageEvent& finalDamageEvent)
{
	finalDamageEvent = inDamageEvent;
}
