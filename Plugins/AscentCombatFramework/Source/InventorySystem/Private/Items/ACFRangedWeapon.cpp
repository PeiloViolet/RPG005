// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Items/ACFRangedWeapon.h"
#include "Items/ACFItem.h"
#include "Items/ACFProjectile.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <Sound/SoundCue.h>

AACFRangedWeapon::AACFRangedWeapon()
{
    ItemInfo.ItemType = EItemType::RangedWeapon;
    ItemInfo.Name = FText::FromString("Base Ranged Weapon");
    ShootingComp = CreateDefaultSubobject<UACFShootingComponent>(TEXT("ShotingComponent"));
}

void AACFRangedWeapon::BeginPlay()
{
    Super::BeginPlay();
    if (ShootingComp) {
        ACharacter* charOwner = Cast<ACharacter>(ItemOwner);
        if (charOwner) {
            ShootingComp->SetupShootingComponent(charOwner, Mesh);
        }
    }
}

void AACFRangedWeapon::Shoot(const FRotator& deltaRot, float charge /*= 1.f*/, TSubclassOf<class AACFProjectile> projectileOverride /*= nullptr);*/)
{
    if (ItemOwner && ShootingComp) {
        const FRotator EyesRotation = ItemOwner->GetControlRotation();
        ShootingComp->ShootAtDirection(EyesRotation + deltaRot, charge, projectileOverride);
    }
}

void AACFRangedWeapon::ShootAtDirection(const FRotator& direction, float charge /*= 1.f*/, TSubclassOf<class AACFProjectile> projectileOverride /* = nullptr);*/)
{
    if (ShootingComp) {
        ShootingComp->ShootAtDirection(direction, charge, projectileOverride);
    }
}

void AACFRangedWeapon::LineTraceShoot(const FRotator& deltaRot)
{
    if (ShootingComp && ItemOwner) {
        const FRotator eyesRotation = ItemOwner->GetControlRotation();
        ShootingComp->LineTraceShootAtDirection(eyesRotation + deltaRot);
    }
}

void AACFRangedWeapon::OnItemEquipped_Implementation()
{
    Super::OnItemEquipped_Implementation();
    if (ShootingComp) {
        ShootingComp->SetupShootingComponent(ItemOwner, Mesh);
    }
}

void AACFRangedWeapon::ShootAtActor(const AActor* target, float randomDeviation /*= 50.f*/, float charge /*= 1.f*/, TSubclassOf<class AACFProjectile> projectileOverride /*= nullptr);*/)
{
    if (ItemOwner && ShootingComp) {
        ShootingComp->ShootAtActor(target, randomDeviation, charge, projectileOverride);
    }
}
