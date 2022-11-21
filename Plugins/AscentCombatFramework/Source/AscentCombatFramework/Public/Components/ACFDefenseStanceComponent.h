// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARSTypes.h"
#include "Game/ACFTypes.h"
#include "Game/ACFDamageType.h"
#include "Components/ACFEquipmentComponent.h"
#include "ACFDefenseStanceComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDefenseStanceChanged, bool, bIsDefending);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCounterAttackTriggered);


UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class ASCENTCOMBATFRAMEWORK_API UACFDefenseStanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFDefenseStanceComponent();

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = ACF)
	void StartDefending();

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = ACF)
	void StopDefending();

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool TryBlockIncomingDamage(const FACFDamageEvent& damageEvent, FGameplayTag& outAction);


	UFUNCTION(BlueprintPure, Category = ACF)
	bool CanBlockDamage(const FACFDamageEvent& damageEvent) const;

	UFUNCTION(BlueprintPure, Category = ACF)
	bool CanStartDefensePosition() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	bool IsInDefensePosition() const {
		return bIsInDefensePosition;
	}

	UFUNCTION(BlueprintPure, Category = ACF)
	FGameplayTag GetBlockAction() const {
		return ActionToBeTriggeredOnBlock;
	}

	UFUNCTION(BlueprintPure, Category = ACF)
	FGameplayTag GetDamagedStatistic() const {
		return DamagedStatistic;
	}

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnDefenseStanceChanged OnDefenseStanceChanged;

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnCounterAttackTriggered OnCounterAttackTriggered;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/*Modifier applied while defending*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	FAttributesSetModifier DefendingModifier;


	/*If the owner can block while unarmed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	bool CanBlockWhenUnarmed = false;

	/*Directions in which incoming hits are blocked*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	EActionDirection BlockDirection;

	/*Action triggered on the owner of this component when an hit is successfully blocked*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	FGameplayTag ActionToBeTriggeredOnBlock;

	/*Action triggered on the owner of this component when the defence is broken*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	FGameplayTag ActionToBeTriggeredOnDefenceBreak;

	/*Types of damages that can be blocked*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	TArray<TSubclassOf<UDamageType>> BlockableDamages;

	/*Damage received during Defense will be  removed from this statistic.
	When this statistic reaches 0, defense will be broken*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	FGameplayTag DamagedStatistic;

	/*Multiplier for the damaged applied to the DamagedStatistic at every damage*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	float DamagedStatisticMultiplier = 1.f;

	/*To start go in DefenseMode, minimum value of the DamagedStatistic to start defense mode*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	float MinimumDamageStatisticToStartBlocking = 5.f;

	/*Action triggered on the owner of this component when an hit is successfully countered*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	FGameplayTag CounterAttackAction;

	/*Types of damages that can be countered*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	TArray<TSubclassOf<UDamageType>> CounterableDamages;
	
	UFUNCTION(BlueprintCallable, Category = ACF)
	void EnableCounterAttackWindow() {
		bCounterGate = true;
	}

	UFUNCTION(BlueprintCallable, Category = ACF)
	void DisableCounterAttackWindow() {
		bCounterGate = false;
	}

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool CanCounterAttack(const FACFDamageEvent& incomingDamage);

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool TryCounterAttack(const FACFDamageEvent& incomingDamage);

	UFUNCTION()
	void OnRep_DefPos();

private:

	UPROPERTY(ReplicatedUsing = OnRep_DefPos)
	bool bIsInDefensePosition = false;

	bool bCounterGate = false;

	class UARSStatisticsComponent* statComp;
	class UACFLocomotionComponent* locComp;
	
	UFUNCTION()
	void HandleEquipmentChanged(const FEquipment& Equipment);
};
