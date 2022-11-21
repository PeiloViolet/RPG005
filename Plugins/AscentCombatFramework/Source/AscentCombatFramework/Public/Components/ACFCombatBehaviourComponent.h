// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFLocomotionComponent.h"
#include "Game/ACFDamageType.h"
#include "Game/ACFTypes.h"
#include <Components/ActorComponent.h>
#include "ACFAITypes.h"
#include "RPGCombatBehaviourData.h"
#include "ACFCombatBehaviourComponent.generated.h"

struct FACFDamageEvent;

/**
 * 
 */

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class ASCENTCOMBATFRAMEWORK_API UACFCombatBehaviourComponent : public UActorComponent
{
	GENERATED_BODY()

	UACFCombatBehaviourComponent();
public: 

	friend class AACFAIController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CombatBehaviourData")
	TObjectPtr<URPGCombatBehaviourData> SourceCombatBehaviourData;
	
	UPROPERTY(BlueprintReadWrite, Category = "CombatBehaviourData")
	TMap<EAICombatState, FActionsGroupStruct> ActionsByCombatState;
	
	UPROPERTY(BlueprintReadOnly, Category = "CombatBehaviourData")
	FActionsGroupStruct CurrentCombatStateActions;
	
	//FActionsGroupStruct* CombatStateActions;

	UFUNCTION(BlueprintCallable, Category = RPG)
	bool RPGExecuteActionByCombatState (EAICombatState CombatState);

	UFUNCTION(BlueprintCallable, Category = RPG)
	void RPGExecuteAINextCombo(FActionsGroupStruct CombatActions);

	UFUNCTION(BlueprintCallable, Category = RPG)
	void RPGExecuteActionByActionsGroupStruct(FActionsGroupStruct CombatActions);
	
	bool ExecuteRandomAction(FActionsGroupStruct CombatActions);
	
	UFUNCTION(BlueprintCallable, Category = RPG)
	bool ActionRequirements(FActionStruct Action);

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool TryExecuteConditionAction();
	
	UFUNCTION(BlueprintPure, Category = ACF)
	bool IsTargetInMeleeRange(AActor* Target);

	UFUNCTION(BlueprintPure, Category = ACF)
	EAICombatState GetBestCombatStateByTargetDistance(float targetDistance);

	UFUNCTION(BlueprintCallable, Category = ACF)
	float GetIdealDistanceByCombatState(EAICombatState combatState) const;

	bool EvaluateCombatState(EAICombatState combatState);

	

private: 

	UFUNCTION(BlueprintCallable, Category = RPG)
	bool ConditionalValidation(const FActionStruct& Action);

	void InitBehavior(class AACFAIController* _controller);

	void TryEquipWeapon();

	void UninitBehavior();

	bool CheckEquipment();

	

	UPROPERTY()
	class AACFCharacter* characterOwner;

	UPROPERTY()
	class AACFAIController* aiController;
	
};
