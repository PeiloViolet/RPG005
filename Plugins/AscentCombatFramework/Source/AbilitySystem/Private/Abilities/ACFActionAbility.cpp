// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "Abilities/ACFActionAbility.h"
#include "Components/ACFActionsManagerComponent.h"
#include <Components/ActorComponent.h>

void UACFActionAbility::OnAbilityActivated_Implementation()
{

	
	UACFActionsManagerComponent* actionsComp = GetActorOwner()->FindComponentByClass<UACFActionsManagerComponent>();
	if (actionsComp) {
		actionsComp->TriggerAction(ActionsToTrigger, AbilityPriority);
	}

	Super::OnAbilityActivated_Implementation();
}
