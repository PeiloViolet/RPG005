// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACMTypes.h"
#include "ACMEffectsDispatcherComponent.generated.h"


UCLASS(ClassGroup=(ACM), meta=(BlueprintSpawnableComponent) )
class COLLISIONSMANAGER_API UACMEffectsDispatcherComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACMEffectsDispatcherComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = ACF)
	void ClientsPlayEffect(const FEffect& effect, class ACharacter* instigator );


	void SpawnSoundAndParticleAttached(const FEffect& effect, class ACharacter* instigator);
	void SpawnSoundAndParticleAtLocation(const FEffect& effect);

public:	

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable,  Category = ACF)
	void ServerPlayReplicatedEffect(const FEffect& effect, class ACharacter* instigator);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void PlayEffectLocally(const FEffect& effect, class ACharacter* instigator);

	void Internal_PlayEffect(class ACharacter* instigator, const FEffect& effect);

};
