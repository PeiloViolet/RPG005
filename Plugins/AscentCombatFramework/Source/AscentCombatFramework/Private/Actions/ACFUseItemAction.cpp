// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "Actions/ACFUseItemAction.h"
#include "Components/ACFEquipmentComponent.h"
#include "Actors/ACFCharacter.h"
#include "Items/ACFWeapon.h"
#include "ACFItemTypes.h"


void UACFUseItemAction::OnActionStarted_Implementation()
{
	bSuccess = false;
}

void UACFUseItemAction::OnNotablePointReached_Implementation()
{
	UseItem();
	bSuccess = true;
}

void UACFUseItemAction::OnActionEnded_Implementation()
{
	if (!bSuccess && bShouldUseIfInterrupted) {
		UseItem();
	}
}

void UACFUseItemAction::UseItem()
{
	if (CharacterOwner)
	{
		UACFEquipmentComponent* equipComp = CharacterOwner->GetEquipmentComponent();
		if (equipComp) {
			equipComp->UseEquippedItemBySlot(ItemSlot);
			if (bTryToEquipOffhand) {
				const AACFWeapon* mainWeap = equipComp->GetCurrentMainWeapon();
				if (mainWeap && mainWeap->GetItemSlot() == ItemSlot &&
					mainWeap->GetHandleType() == EHandleType::OneHanded) {
					equipComp->UseEquippedItemBySlot(OffHandSlot);
				}   
			}
		
		}
			
	}
}

bool UACFUseItemAction::CanExecuteAction_Implementation(class AACFCharacter* owner)
{
	if (!owner)
		return false;

	const UACFEquipmentComponent* equipComp = owner->GetEquipmentComponent();

	if (equipComp)
	{
		return equipComp->HasAnyItemInEquipmentSlot(ItemSlot);
	}
	return false;
}


