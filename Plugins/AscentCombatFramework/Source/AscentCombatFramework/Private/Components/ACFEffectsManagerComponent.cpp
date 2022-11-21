// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Components/ACFEffectsManagerComponent.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFDamageHandlerComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include <CollisionQueryParams.h>
#include <PhysicalMaterials/PhysicalMaterial.h>



// Sets default values for this component's properties
UACFEffectsManagerComponent::UACFEffectsManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    DamageEffectsByHitReaction.Add(FEffectByDamageType());
}

// Called when the game starts
void UACFEffectsManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    CharacterOwner = Cast<AACFCharacter>(GetOwner());

    UACFDamageHandlerComponent* DamageHanlder = GetOwner()->FindComponentByClass<UACFDamageHandlerComponent>();

    if (DamageHanlder) {
        DamageHanlder->OnDamageReceived.AddDynamic(this, &UACFEffectsManagerComponent::HandleDamageReceived);
    }
}

void UACFEffectsManagerComponent::TriggerFootstepFX(FName footBone /*= NAME_None*/)
{
    FEffect fxToPlay;
    if (bCheckForSurface) {
        const FEffect* fx = FootstepEffectsBySurface.Find(GetCurrentTerrain());
        if (!fx) {
            return;
        } else {
            fxToPlay = *fx;
        }
    } else {
        fxToPlay = DefaultFootstepFX;
    }

    FVector ownerLocation;

    if (footBone != NAME_None) {
        fxToPlay.SocketOrBoneName = footBone;
        ownerLocation = CharacterOwner->GetMesh()->GetSocketLocation(footBone);
    } else {
        ownerLocation = CharacterOwner->GetActorLocation();
    }

    const UACFLocomotionComponent* locComp = CharacterOwner->FindComponentByClass<UACFLocomotionComponent>();

    if (locComp) {
        float* noise = FootstepNoiseByLocomotionState.Find(locComp->GetCurrentLocomotionState());
        if (noise) {
            fxToPlay.NoiseEmitted = *noise;
        }
    }

    UACFFunctionLibrary::PlayActionEffectLocally(fxToPlay, CharacterOwner, this);
}

EPhysicalSurface UACFEffectsManagerComponent::GetCurrentTerrain()
{
    ensure(CharacterOwner);
    if (CharacterOwner) {
        FCollisionQueryParams outTraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, CharacterOwner);

        outTraceParams.bTraceComplex = true;
        outTraceParams.bReturnPhysicalMaterial = true;

        const FVector Start = CharacterOwner->GetActorLocation();
        const FVector End = Start + (FVector(0, 0, 1) * TraceLengthByActorLocation * -1);

        //Re-initialize hit info
        FHitResult outTerrain;

        const bool bHit = GetWorld()->LineTraceSingleByObjectType(
            outTerrain, //result
            Start, //start
            End, //end
            ECC_WorldStatic, //collision channel
            outTraceParams);

        if (bHit) {
            const UPhysicalMaterial* PhysicsMtl = outTerrain.PhysMaterial.Get();
            const EPhysicalSurface surface = PhysicsMtl->SurfaceType;
            return surface;
        }
    }

    return EPhysicalSurface::SurfaceType_Max;
}

void UACFEffectsManagerComponent::HandleDamageReceived(const FACFDamageEvent& damageEvent)
{
    OnDamageImpactReceived(damageEvent);
}

void UACFEffectsManagerComponent::OnDamageImpactReceived_Implementation(const FACFDamageEvent& damageEvent)
{
    PlayHitReactionEffect(damageEvent.HitResponseAction, damageEvent.DamageClass);
}

void UACFEffectsManagerComponent::PlayCurrentActionEffect(const FEffect& effect)
{
    FEffect outEffect = effect;
	const UACFDamageHandlerComponent* damageComp = GetOwner()->FindComponentByClass< UACFDamageHandlerComponent>();
    if(damageComp){
		const FVector finalPos = effect.RelativeOffset.GetLocation() + damageComp->GetLastDamageInfo().hitResult.Location;
        outEffect.RelativeOffset.SetLocation(finalPos);

		UACFFunctionLibrary::PlayActionEffect(outEffect, CharacterOwner, this);
    }
    else {
		UE_LOG(LogTemp, Error, TEXT("No Damage Handler comp in your character!"));
    }
}

void UACFEffectsManagerComponent::PlayHitReactionEffect(const FGameplayTag& HitRection, const TSubclassOf<class UDamageType>& DamageType, ESpawnFXLocation locationSpawn /*= ESpawnFXLocation::ELastDamageLocation*/, const FName& boneOrSocketName /* = NAME_None*/)
{
    FEffectByDamageType* effectList;
    if (HitRection == FGameplayTag()) {
        effectList = DamageEffectsByHitReaction.FindByKey(UACFFunctionLibrary::GetDefaultHitState());
    } else {
        effectList = DamageEffectsByHitReaction.FindByKey(HitRection);
    }

    if (effectList) {
       FEffect* effect = effectList->EffectByDamageType.Find(DamageType);

        if (effect) {
			effect->SocketOrBoneName = boneOrSocketName;
			effect->SpawnLocation = locationSpawn;
            PlayCurrentActionEffect(*effect);
            return;
        }

        for (auto& currentDam : effectList->EffectByDamageType) {
            if (DamageType->IsChildOf(currentDam.Key)) {
                currentDam.Value.SocketOrBoneName = boneOrSocketName;
                currentDam.Value.SpawnLocation = locationSpawn;
                PlayCurrentActionEffect(currentDam.Value);
                return;
            }
        }
    }
}
