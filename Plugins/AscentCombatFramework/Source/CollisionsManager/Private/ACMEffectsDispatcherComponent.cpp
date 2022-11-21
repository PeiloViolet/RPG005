// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACMEffectsDispatcherComponent.h"
#include <Kismet/GameplayStatics.h>
#include "ACMTypes.h"
#include <Sound/SoundBase.h>
#include <Sound/SoundCue.h>
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"


// Sets default values for this component's properties
UACMEffectsDispatcherComponent::UACMEffectsDispatcherComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


// Called when the game starts
void UACMEffectsDispatcherComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
}

void UACMEffectsDispatcherComponent::ClientsPlayEffect_Implementation(const FEffect& effect, class ACharacter* instigator)
{
	Internal_PlayEffect(instigator, effect);
}

bool UACMEffectsDispatcherComponent::ClientsPlayEffect_Validate(const FEffect& effect, class ACharacter* instigator)
{
	return true;
}

void UACMEffectsDispatcherComponent::ServerPlayReplicatedEffect_Implementation(const FEffect& effect, class ACharacter* instigator)
{
	if (instigator)
	{
		instigator->MakeNoise(effect.NoiseEmitted, instigator, instigator->GetActorLocation(), effect.NoiseEmitted );
		ClientsPlayEffect(effect, instigator);
	}
}

bool UACMEffectsDispatcherComponent::ServerPlayReplicatedEffect_Validate(const FEffect& effect, class ACharacter* instigator)
{
	return true;
}

void UACMEffectsDispatcherComponent::PlayEffectLocally(const FEffect& effect, class ACharacter* instigator)
{
	Internal_PlayEffect(instigator, effect);
}

void UACMEffectsDispatcherComponent::Internal_PlayEffect(class ACharacter* instigator, const FEffect& effect)
{
	if (instigator)	{
		const FEffect& damageEffect = effect;
		switch (effect.SpawnLocation)
		{
		case ESpawnFXLocation::ESpawnOnActorLocation:
		case ESpawnFXLocation::ESpawnAttachedToSocketOrBone:
			SpawnSoundAndParticleAttached(effect, instigator);
			break;
		case ESpawnFXLocation::ESpawnAtLocation:
			SpawnSoundAndParticleAtLocation(damageEffect);
			break;
		default: 
			SpawnSoundAndParticleAttached(effect, instigator);
			break;
		}
	}
}

void UACMEffectsDispatcherComponent::SpawnSoundAndParticleAttached(const FEffect& effect, class ACharacter* instigator)
{
	if (effect.ActionParticle)	{
		UGameplayStatics::SpawnEmitterAttached(effect.ActionParticle, instigator->GetMesh(), effect.SocketOrBoneName,
		effect.RelativeOffset.GetLocation(), effect.RelativeOffset.GetRotation().Rotator(), effect.RelativeOffset.GetScale3D());
	}
	
	if (effect.ActionSound){
		UGameplayStatics::SpawnSoundAttached(effect.ActionSound, instigator->GetMesh(), effect.SocketOrBoneName);
	}

	if (effect.NiagaraParticle) {
		UNiagaraFunctionLibrary::SpawnSystemAttached(effect.NiagaraParticle, instigator->GetMesh(), effect.SocketOrBoneName,
			effect.RelativeOffset.GetLocation(), effect.RelativeOffset.GetRotation().Rotator(), effect.RelativeOffset.GetScale3D(),
			EAttachLocation::SnapToTarget, true, ENCPoolMethod::None);
	}
}

void UACMEffectsDispatcherComponent::SpawnSoundAndParticleAtLocation(const FEffect& effect)
{
	if (effect.ActionParticle)	{
		UGameplayStatics::SpawnEmitterAtLocation(this, effect.ActionParticle, effect.RelativeOffset.GetLocation(), 
			effect.RelativeOffset.GetRotation().Rotator(), effect.RelativeOffset.GetScale3D());
	}
	
	if (effect.ActionSound)	{
		UGameplayStatics::SpawnSoundAtLocation(this, effect.ActionSound, effect.RelativeOffset.GetLocation());
	}

	if (effect.NiagaraParticle) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, effect.NiagaraParticle, effect.RelativeOffset.GetLocation(),
			effect.RelativeOffset.GetRotation().Rotator(), effect.RelativeOffset.GetScale3D());
	}
}

