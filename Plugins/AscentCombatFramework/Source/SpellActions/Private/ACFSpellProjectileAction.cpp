// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACFSpellProjectileAction.h"
#include "Components/ACFShootingComponent.h"
#include <GameFramework/Pawn.h>
#include "Items/ACFProjectile.h"

void UACFSpellProjectileAction::OnNotablePointReached_Implementation()
{
	Super::OnNotablePointReached_Implementation();

	if (CharacterOwner && ProjectileClass) {
		UACFShootingComponent* shootComp = CharacterOwner->FindComponentByClass<UACFShootingComponent>();
		if (shootComp) {
			FVector location;
			FRotator rot;
			CharacterOwner->GetActorEyesViewPoint(location, rot);
			shootComp->ShootAtDirection(rot, 1.f, ProjectileClass);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("No shootingComp on your character! - UACFSpellProjectileAction"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No shooting COmp on your character! - UACFSpellProjectileAction"));
	}
}
