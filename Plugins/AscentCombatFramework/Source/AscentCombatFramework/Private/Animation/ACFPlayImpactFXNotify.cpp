// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "Animation/ACFPlayImpactFXNotify.h"
#include <Components/SkeletalMeshComponent.h>
#include "Components/ACFEffectsManagerComponent.h"

void UACFPlayImpactFXNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp) {
		const AActor* owner = MeshComp->GetOwner();
		if (owner) {
			UACFEffectsManagerComponent* effectsComp = owner->FindComponentByClass<UACFEffectsManagerComponent>();
			if (effectsComp) {
				effectsComp->PlayHitReactionEffect(HitReaction, DamageReceivedType, SpawnLocation, SocketOrBoneLocation);
			}
		}
	}
}
