// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACFAbilitySystemComponent.h"
#include "ACFAbilityFunctionLibrary.h"
#include "Abilities/ACFPassiveAbility.h"

// Sets default values for this component's properties
UACFAbilitySystemComponent::UACFAbilitySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UACFAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	if (bAutoInitialize) {
		InitAbilities();
	}

}

void UACFAbilitySystemComponent::GrantAbility(const FName& abilityName)
{
	FAbilityConfig outAbility;
	if (UACFAbilityFunctionLibrary::GetAbilityFromDB(abilityName, outAbility, this) && outAbility.Ability) {
		UACFAbility* newAbility = NewObject< UACFAbility>(outAbility.Ability);
		if (newAbility) {

			currentlyAvailableAbilities.Add(abilityName, outAbility);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("INVALID ABILITY! - UACFAbilitySystemComponent::GrantAbility"));
	}

}

void UACFAbilitySystemComponent::RemoveAbility(const FName& abilityName)
{
	if (HasAbility(abilityName)) {
		if (HasActiveAbility(abilityName)) {
			currentlyActiveAbilities.Remove(abilityName);
		}
		currentlyAvailableAbilities.Remove(abilityName);
	}
}

bool UACFAbilitySystemComponent::TryUseAbility(const FName& abilityName)
{
	if (HasAbility(abilityName)) {
		UACFAbility* ability = nullptr;
		if (!HasActiveAbility(abilityName)) {
			 FAbilityConfig* abilityConf = currentlyAvailableAbilities.Find(abilityName);
			 if (abilityConf && abilityConf->Ability) {
				 ability = NewObject<UACFAbility>(abilityConf->Ability, abilityConf->Ability);
			 }
		}
		else {
			ability = currentlyActiveAbilities.FindChecked(abilityName);
		}
		 
		if (ability && ability->CanBeActivated()) {
			currentlyActiveAbilities.Add(abilityName, ability);
			ability->Internal_OnActivated(GetOwner());		
			if (ability->GetAbilityType() == EAbilityType::EActive) {
				DeactivateAbility(abilityName);
			}
			return true;

		}
	}
	return false;
}

void UACFAbilitySystemComponent::DeactivateAbility(const FName& abilityName)
{
	if (HasAbility(abilityName) && HasActiveAbility(abilityName)) {
		UACFAbility* ability = currentlyActiveAbilities.FindChecked(abilityName);
		ability->OnAbilityDeactivated();
		currentlyActiveAbilities.Remove(abilityName);
	}
}

void UACFAbilitySystemComponent::InitAbilities()
{
	for (auto abilityName : StartingAbilities) {
		GrantAbility(abilityName);

	}

	if (bAutoActivatePassiveAbilities) {
		for (auto ability: currentlyAvailableAbilities) {
			if (ability.Value.Ability && ability.Value.Ability->IsChildOf(UACFPassiveAbility::StaticClass())) {
				TryUseAbility(ability.Key);
			}
		}
	}
}

bool UACFAbilitySystemComponent::GrantAndUseAbility(const FName& abilityName)
{	
	GrantAbility(abilityName);
	return TryUseAbility(abilityName);
}

bool UACFAbilitySystemComponent::HasAbility(const FName& abilityName) const
{
	return currentlyAvailableAbilities.Contains(abilityName);
}

bool UACFAbilitySystemComponent::HasActiveAbility(const FName& abilityName) const
{
	return currentlyActiveAbilities.Contains(abilityName);
}

