// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/ACFDamageType.h"
#include "Game/ACFDamageCalculation.h"
#include "Actors/ACFCharacter.h"
#include "ACFDamageHandlerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);



UCLASS(Blueprintable, ClassGroup=(ACF), meta=(BlueprintSpawnableComponent) )
class ASCENTCOMBATFRAMEWORK_API UACFDamageHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFDamageHandlerComponent();

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE	FACFDamageEvent GetLastDamageInfo() const {
		return LastDamageReceived;
	}

	/*assign the correct collision channel to the damage meshes from the team*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	void InitializeDamageCollisions(ETeam combatTeam);

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnDamageReceived OnDamageReceived;

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnCharacterDeath OnOwnerDeath;


public:	


	float TakeDamage(class AActor* damageReceiver, float Damage, FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		class AActor* DamageCauser);


	UFUNCTION(BlueprintPure, Category = ACF)
		bool GetIsAlive() const {
		return bIsAlive;
	}

protected:

	virtual void BeginPlay() override;

	/*The actions that should be triggered when this ai got hit and the conditional triggers.
	Used to define automatic actions like Dodging/parrying/counterattacking or just playing hit animations*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	TArray<FOnHitActionChances> HitResponseActions;


private:


	void ConstructDamageReceived(AActor* DamagedActor, float Damage, class AController* InstigatedBy,
			FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			TSubclassOf<UDamageType> DamageType, AActor* DamageCauser);


	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = ACF)
	void ClientsReceiveDamage(const FACFDamageEvent& damageEvent);

	UPROPERTY()
	class UACFDamageCalculation* DamageCalculator;

	UPROPERTY()
	FACFDamageEvent LastDamageReceived;

	UFUNCTION()
	void HandleStatReachedZero(FGameplayTag stat);
	
	FGameplayTag EvaluateHitResponse(const FACFDamageEvent& damageEvent);
	bool EvaluetHitResponseAction(const FOnHitActionChances& action, const FACFDamageEvent& damageEvent);
	void AssignCollisionProfile(const TEnumAsByte<ECollisionChannel> channel);

	UPROPERTY(Replicated)
	bool bIsAlive = true;

	ETeam inCombatTeam;
};
