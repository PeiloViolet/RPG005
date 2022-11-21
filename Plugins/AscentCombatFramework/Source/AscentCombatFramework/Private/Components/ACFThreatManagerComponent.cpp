// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "Components/ACFThreatManagerComponent.h"
#include "Interfaces/ACFEntityInterface.h"
#include "Actors/ACFCharacter.h"
#include "Actors/ACFActor.h"
#include <GameFramework/Actor.h>

// Sets default values for this component's properties
UACFThreatManagerComponent::UACFThreatManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UACFThreatManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	DefaultThreatMap.Add(AACFActor::StaticClass(), 5.f);
	DefaultThreatMap.Add(AACFCharacter::StaticClass(), 10.f);
}



void UACFThreatManagerComponent::UpdateMaxThreat()
{
	AActor* newMax = GetActorWithHigherThreat();
	if ( newMax != maxThreatening) {
		maxThreatening = newMax;
		OnNewMaxThreateningActor.Broadcast(maxThreatening);
	}
}

void UACFThreatManagerComponent::AddThreat(AActor* threatening, float threat)
{
	if (!threatening) {
		return;
	}

	if (threatening == GetOwner()) {
		return;
	}
	
	IACFEntityInterface* entity = Cast<IACFEntityInterface>(threatening);
	if (!entity) {
		return;
	}

	if (IACFEntityInterface::Execute_IsEntityAlive(threatening))
	{
		if (ThreatMap.Contains(threatening))
		{
			threat += ThreatMap.FindAndRemoveChecked(threatening);
		}
		const float threatMult = 
		ThreatMap.Add(threatening, threat);
		UpdateMaxThreat();
	}
	else {
		RemoveThreatening(threatening);
	}
	
}


void UACFThreatManagerComponent::RemoveThreat(class AActor* threatening, float threat)
{
	if (!threatening) {
		return;
	}

	IACFEntityInterface* entity = Cast<IACFEntityInterface>(threatening);
	if (!entity) {
		return;
	}

	if (IACFEntityInterface::Execute_IsEntityAlive(threatening))
	{
		if (ThreatMap.Contains(threatening))
		{
			threat -= ThreatMap.FindAndRemoveChecked(threatening);

			if (threat >= 0) {
				ThreatMap.Add(threatening, threat);
				UpdateMaxThreat();
			}
		}
	}
	else {
		RemoveThreatening(threatening);
	}
}

class AActor* UACFThreatManagerComponent::GetActorWithHigherThreat()
{
	float maxThreat = -1.f;
	AActor* target = nullptr;
	TArray< AActor*> pendingDelete;

	for (const auto& elem : ThreatMap)
	{
		IACFEntityInterface* entity = Cast<IACFEntityInterface>(elem.Key);

		if (!entity || !elem.Key || !IACFEntityInterface::Execute_IsEntityAlive(elem.Key))
		{
			pendingDelete.Add(elem.Key);
		}
		else if (elem.Value > maxThreat)
		{
			maxThreat = elem.Value;
			target = elem.Key;
		}
	}

	for (const auto* toDelete : pendingDelete)
	{
		ThreatMap.Remove(toDelete);
	}

	return target;
}

float UACFThreatManagerComponent::GetThreatMultiplierForActor(AActor* threatening)
{
	const float* threat = ThreatMultiplierMap.Find(threatening->GetClass());

	if (threat) {
		return *threat;
	}

	for (auto& elem : ThreatMultiplierMap) {
		if (threatening->IsA(elem.Key)) {
			return elem.Value;
		}
	}
	return 1.f;
}

bool UACFThreatManagerComponent::IsThreatening(class AActor* threatening) const
{
	return ThreatMap.Contains(threatening);
}

float UACFThreatManagerComponent::GetDefaultThreatForActor(AActor* threatening)
{
	const float* threat = DefaultThreatMap.Find(threatening->GetClass());

	if (threat) {
		return *threat;
	}

	for (const auto& elem : DefaultThreatMap) {
		if (threatening->IsA(elem.Key)) {
			return elem.Value;
		}
	}
	return 0.f;
}

void UACFThreatManagerComponent::RemoveThreatening(AActor* threatening)
{
	if (ThreatMap.Contains(threatening))
	{
		ThreatMap.FindAndRemoveChecked(threatening);
	}

	if (maxThreatening == threatening) {
		maxThreatening = nullptr;
	}
}
