// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Components/ACFDefenseStanceComponent.h"
#include "ARSStatisticsComponent.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFEquipmentComponent.h"
#include "Components/ACFLocomotionComponent.h"
#include "Items/ACFWeapon.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UACFDefenseStanceComponent::UACFDefenseStanceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	// ...
}

void UACFDefenseStanceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UACFDefenseStanceComponent, bIsInDefensePosition);
}


// Called when the game starts
void UACFDefenseStanceComponent::BeginPlay()
{
	Super::BeginPlay();

	statComp = GetOwner()->FindComponentByClass<UARSStatisticsComponent>();
	ensure(statComp);
	locComp = GetOwner()->FindComponentByClass<UACFLocomotionComponent>();

	UACFEquipmentComponent* equipComp = GetOwner()->FindComponentByClass<UACFEquipmentComponent>();
	if (equipComp) {
		equipComp->OnEquipmentChanged.AddDynamic(this, &UACFDefenseStanceComponent::HandleEquipmentChanged);
	}
}

void UACFDefenseStanceComponent::HandleEquipmentChanged(const FEquipment& Equipment)
{
	if (bIsInDefensePosition) {
		StopDefending();
	}
}

bool UACFDefenseStanceComponent::CanStartDefensePosition() const
{
	if (!statComp) {
		return false;
	}

	if (statComp->GetCurrentValueForStatitstic(DamagedStatistic) < MinimumDamageStatisticToStartBlocking) {
		return false;
	}

	const UACFEquipmentComponent* equipComp = GetOwner()->FindComponentByClass<UACFEquipmentComponent>();
	if (!equipComp) {
		return CanBlockWhenUnarmed;
	}

	const AACFWeapon* currWeap = equipComp->GetCurrentMainWeapon();
	if (!currWeap) {
		return CanBlockWhenUnarmed;
	}
	if (currWeap->GetCanBeUsedForBlock()) {
		return true;
	}

	const AACFWeapon* offhand = equipComp->GetCurrentOffhandWeapon();
	if (!offhand) {
		return false;
	}

	return offhand->GetCanBeUsedForBlock();
}
void UACFDefenseStanceComponent::StartDefending_Implementation()
{
	if (CanStartDefensePosition()) {
		if (locComp) {
			locComp->StartBlendingAdditiveAnimation();
		}
		bIsInDefensePosition = true;
		statComp->AddAttributeSetModifier(DefendingModifier);
		OnDefenseStanceChanged.Broadcast(bIsInDefensePosition);
		const UACFEquipmentComponent* equipComp = GetOwner()->FindComponentByClass<UACFEquipmentComponent>();
		if (equipComp) {
			const AACFWeapon* weap = equipComp->GetCurrentMainWeapon();
			if (weap && weap->GetCanBeUsedForBlock()) {
				statComp->AddAttributeSetModifier(weap->GetDefenseAttributeSetModifier());
			}
			const AACFWeapon* secondweap = equipComp->GetCurrentOffhandWeapon();
			if (secondweap && secondweap->GetCanBeUsedForBlock()) {
				statComp->AddAttributeSetModifier(secondweap->GetDefenseAttributeSetModifier());
			}
		}
	}
}

bool UACFDefenseStanceComponent::StartDefending_Validate()
{
	return true;
}

void UACFDefenseStanceComponent::StopDefending_Implementation()
{
	if (!statComp) {
		return;
	}
	if (!bIsInDefensePosition) {
		return;
	}

	if (locComp) {
		locComp->StopBlendingAdditiveAnimation();
	}
	bIsInDefensePosition = false;
	statComp->RemoveAttributeSetModifier(DefendingModifier);
	OnDefenseStanceChanged.Broadcast(bIsInDefensePosition);
	const UACFEquipmentComponent* equipComp = GetOwner()->FindComponentByClass<UACFEquipmentComponent>();
	if (equipComp) {
		const AACFWeapon* weap = equipComp->GetCurrentMainWeapon();
		if (weap && weap->GetCanBeUsedForBlock()) {
			statComp->RemoveAttributeSetModifier(weap->GetDefenseAttributeSetModifier());
		}
		const AACFWeapon* secondweap = equipComp->GetCurrentOffhandWeapon();
		if (secondweap && secondweap->GetCanBeUsedForBlock()) {
			statComp->RemoveAttributeSetModifier(secondweap->GetDefenseAttributeSetModifier());
		}
	}
}

bool UACFDefenseStanceComponent::StopDefending_Validate()
{
	return true;
}

bool UACFDefenseStanceComponent::TryBlockIncomingDamage(const FACFDamageEvent& damageEvent, FGameplayTag& outAction)
{
	if (!CanBlockDamage(damageEvent)) {
		return false;
	}

	const bool bUsingDamagedStat = DamagedStatistic != FGameplayTag();
	if (bUsingDamagedStat) {
		statComp->ModifyStat(FStatisticModifier(DamagedStatistic, -damageEvent.FinalDamage * DamagedStatisticMultiplier));
	}

	if (statComp->GetCurrentValueForStatitstic(DamagedStatistic) > 0.f || !bUsingDamagedStat) {
		outAction = ActionToBeTriggeredOnBlock;

		UACFActionsManagerComponent* actionsManager = damageEvent.DamageDealer->FindComponentByClass<UACFActionsManagerComponent>();
		if (actionsManager) {
			actionsManager->TriggerAction(CounterAttackAction, EActionPriority::EHigh);
		}
		return true;
	}
	else {
		outAction = ActionToBeTriggeredOnDefenceBreak;
		StopDefending();
		return false;
	}

	return false;
}

bool UACFDefenseStanceComponent::CanBlockDamage(const FACFDamageEvent& damageEvent) const
{
	if (!statComp) {
		UE_LOG(LogTemp, Error, TEXT("Missing Stat Component! - UACFDefenseStanceComponent::CanBlockDamage"));
		return false;
	}

	if (!bIsInDefensePosition) {
		return false;
	}

	if ((uint8)damageEvent.DamageDirection != (uint8)BlockDirection && BlockDirection != EActionDirection::EveryDirection) {
		return false;
	}

	const UACFActionsManagerComponent* ownerActions = GetOwner()->FindComponentByClass<UACFActionsManagerComponent>();
	//you can't block while doing other actions
	if (!ownerActions->CanExecuteAction(ActionToBeTriggeredOnBlock)) {
		UE_LOG(LogTemp, Warning, TEXT("Impossible to execute block action! - UACFDefenseStanceComponent::CanBlockDamage"));
		return false;
	}

	return BlockableDamages.Contains(damageEvent.DamageClass);
}

bool UACFDefenseStanceComponent::CanCounterAttack(const FACFDamageEvent& incomingDamage)
{
	if (!bCounterGate) {
		return false;
	}

	const UACFActionsManagerComponent* actionsManager = GetOwner()->FindComponentByClass<UACFActionsManagerComponent>();
	if (!actionsManager) {
		return false;
	}

	if (!actionsManager->CanExecuteAction(CounterAttackAction)) {
		return false;
	}
	if (!CounterableDamages.Contains(incomingDamage.DamageClass)) {
		return false;
	}

	if (!incomingDamage.DamageDealer) {
		return false;
	}

	const IACFEntityInterface* acfEntity = Cast<IACFEntityInterface>(incomingDamage.DamageDealer);
	if (acfEntity && !IACFEntityInterface::Execute_IsEntityAlive(incomingDamage.DamageDealer)) {
		return false;
	}

	return true;
}

bool UACFDefenseStanceComponent::TryCounterAttack(const FACFDamageEvent& incomingDamage)
{
	if (!CanCounterAttack(incomingDamage)) {
		return false;
	}
	UACFActionsManagerComponent* actionsManager = GetOwner()->FindComponentByClass<UACFActionsManagerComponent>();
	if (actionsManager) {
		actionsManager->TriggerAction(CounterAttackAction, EActionPriority::EHighest);
		OnCounterAttackTriggered.Broadcast();
	}
	return true;
}

void UACFDefenseStanceComponent::OnRep_DefPos()
{
	OnDefenseStanceChanged.Broadcast(bIsInDefensePosition);
}
