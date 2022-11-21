// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Actions/ACFShootAction.h"
#include "Actors/ACFCharacter.h"
#include "Items/ACFRangedWeapon.h"

UACFShootAction::UACFShootAction()
{
}

bool UACFShootAction::CanExecuteAction_Implementation(class AACFCharacter* owner)
{
    if (owner) {
        const AACFWeapon* mainWeapon = owner->GetEquipmentComponent()->GetCurrentMainWeapon();
        if (mainWeapon) {
            const UACFShootingComponent* shootingComp = mainWeapon->FindComponentByClass<UACFShootingComponent>();
            if (shootingComp) {
                return shootingComp->CanShoot();
            };
        }
    }
    return false;
}

void UACFShootAction::OnNotablePointReached_Implementation()
{
    Super::OnNotablePointReached_Implementation();

    if (CharacterOwner) {
        const UACFEquipmentComponent* equip = CharacterOwner->GetEquipmentComponent();
        AACFWeapon* weap = equip->GetCurrentMainWeapon();
        AACFRangedWeapon* rangedWeapon = Cast<AACFRangedWeapon>(weap);

        if (rangedWeapon) {
            switch (rangedWeapon->GetShootingType()) {
            case EShootingType::ELineTrace:
                rangedWeapon->LineTraceShoot(DeltaDirection);
                break;
            case EShootingType::EProjectile:
                rangedWeapon->Shoot(DeltaDirection, 1.f, ProjectileOverride);
                break;
            }
        }
    }
}
