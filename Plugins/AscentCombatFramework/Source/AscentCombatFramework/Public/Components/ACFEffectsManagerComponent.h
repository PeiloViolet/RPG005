// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Engine/EngineTypes.h>
#include "Components/ACFActionsManagerComponent.h"
#include "Game/ACFDamageType.h"
#include "Components/ACFLocomotionComponent.h"
#include "ACFEffectsManagerComponent.generated.h"


UCLASS(Blueprintable, ClassGroup = (ACF), meta = (BlueprintSpawnableComponent))
class ASCENTCOMBATFRAMEWORK_API UACFEffectsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFEffectsManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UPROPERTY(BlueprintReadOnly, Category = ACF)
	class ACharacter* CharacterOwner;

	/*Enable terrain material check for footspeps*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF | Footstep")
	bool bCheckForSurface = false;

	/*Effect played on Footstep when CheckForSurface is DISABLED*/
	UPROPERTY(BlueprintReadOnly, meta = (EditCondition = "!bCheckForSurface" ), EditDefaultsOnly, Category = "ACF | Footstep")
	FEffect DefaultFootstepFX;

	/*Effects played on Footstep when CheckForSurface is ENABLED, depending on the actual
	terrain surface physical material*/
	UPROPERTY(BlueprintReadOnly, meta = (EditCondition = "bCheckForSurface"), EditDefaultsOnly, Category = "ACF | Footstep")
	TMap<TEnumAsByte<EPhysicalSurface>, FEffect> FootstepEffectsBySurface;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF | Footstep")
	float TraceLengthByActorLocation = 200.f;

	/*The noise emitted while moving by this character. This noise is used as a check 
	for AI Perceptions*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF | Footstep")
	TMap<ELocomotionState, float> FootstepNoiseByLocomotionState;

	/*Effects triggered when this character gets hit depending on the DamageType Received.
	Location will always be the impact point*/
	UPROPERTY(BlueprintReadOnly, meta = (TitleProperty = "TagName"), EditDefaultsOnly, Category = "ACF | Damages")
	TArray<FEffectByDamageType> DamageEffectsByHitReaction;

public:
	UFUNCTION(BlueprintCallable, Category = ACF)
	void TriggerFootstepFX(FName footBone = NAME_None);

	UFUNCTION(BlueprintCallable, Category = ACF)
	EPhysicalSurface GetCurrentTerrain();

	UFUNCTION(BlueprintNativeEvent, Category = ACF)
	void OnDamageImpactReceived(const FACFDamageEvent& damageEvent);
	virtual void OnDamageImpactReceived_Implementation(const FACFDamageEvent& damageEvent);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void PlayHitReactionEffect(const FGameplayTag& HitRection, const TSubclassOf<class UDamageType>& DamageType, ESpawnFXLocation locationSpawn = ESpawnFXLocation::ESpawnAtLocation,  const FName& boneOrSocketName = NAME_None);

private: 

	UFUNCTION()
	void HandleDamageReceived(const FACFDamageEvent& damageEvent);

	void PlayCurrentActionEffect(const FEffect& effect);

};
