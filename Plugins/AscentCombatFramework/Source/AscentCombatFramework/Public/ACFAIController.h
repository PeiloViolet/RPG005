// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "Game/ACFDamageType.h"
#include "Actors/ACFCharacter.h"
#include "ACFAITypes.h"
#include <Perception/AIPerceptionTypes.h>
#include "Components/ACFLocomotionComponent.h"
#include <DetourCrowdAIController.h>
#include "Groups/ACFGroupAgentInterface.h"
#include "ACFBaseAIController.h"
#include "ACFAIController.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIStateChanged, const EAIState, AIState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAICombatStateChanged, const EAICombatState, AICombatState);

UCLASS()
class ASCENTCOMBATFRAMEWORK_API AACFAIController : public AACFBaseAIController, public IACFGroupAgentInterface
 {
	GENERATED_BODY()

protected:

	virtual void PostInitProperties() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type reason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UBlackboardComponent* BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UACFCombatBehaviourComponent* CombatBehaviorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UACFCommandsManagerComponent* CommandsManagerComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UATSAITargetComponent* TargetingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UACFThreatManagerComponent* ThreatComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	class AACFCharacter* CharacterOwned;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	int32 GroupIndex;

	/*Default AI state for this agent*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	EAIState DefaultState;
	
	/*If set to true this AI will try to come back to the Lead Position once it is over MaxDistanceFromHome*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	bool bBoundToHome = false;

	/*If bBoundToHome  is true, once the AI is over this distance from his lead/home position, he will try to come back home*/
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bBoundToHome), BlueprintReadOnly, Category = ACF)
	float MaxDistanceFromHome = 5000.f;

	/*If set to true this AI will be teleported back near the Lead Position once it is over TeleportHomeTriggerDistance*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	bool bTeleportHome = false;

	/*Distance to trigger the teleport*/
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bTeleportHome), BlueprintReadOnly, Category = ACF)
	float TeleportHomeTriggerDistance = 8500.f;

	/*Radius in which the controlled pawn is spawned near the lead*/
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = bTeleportHome), BlueprintReadOnly, Category = ACF)
	float TeleportHomeNearRadius = 2500.f;

	/*Distance for which the controlled pawn stop fighting against his target*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	float LoseTargetDistance = 3500.f;

	/*Indicates if this agent attacks an enemy on perception*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	bool bIsAggressive = true;

	/*Indicates if this agent  should react once he gets hit*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	bool bShouldReactOnHit = true;

	/*Indicates the default locomotion state of this AI for each AI state*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	TMap<EAIState, ELocomotionState> LocomotionStateByAIState;

public:

	AACFAIController(const FObjectInitializer& ObjectInitializer);

	/// <summary>
	/// /Groupable Entity Interface
	/// </summary>
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
	class UACFGroupAIComponent* GetOwningGroup();
	virtual class UACFGroupAIComponent* GetOwningGroup_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
	bool IsPartOfGroup();
	virtual bool IsPartOfGroup_Implementation() override;
	/// <summary>
	/// /End Groupable Entity
	/// </summary>
 
    UFUNCTION(BlueprintCallable, Category = ACF)
	void SetPatrolPath(class AACFPatrolPath* inPatrol);

	UFUNCTION(BlueprintPure, Category = ACF)
	class AACFPatrolPath* GetPatrolPath() const {
		return patrolPath;
	}

	
private:
	void EnableCharacterComponents(bool bEnabled);

	UPROPERTY()
	class UACFGroupAIComponent* GroupOwner;


	UPROPERTY()
	int32 patrolIndex = 0;

	UPROPERTY()
	AACFPatrolPath* patrolPath;

	bool bIsReversed = false;

	uint8 commandDurationTimeKey,
		targetActorKey,
		targetActorDistanceKey,
		targetPointLocationKey,
		leadDistanceKey,
		aiStateKey,
		groupLeaderKey, targetDistanceKey,
		comboCounterKey, combatStateKey, isPausedKey;


	UFUNCTION()
	void HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void HandleMaxThreatUpdated(AActor* newMax);

	UFUNCTION()
	void HandleCharacterHit(const FACFDamageEvent& damageReceived);

	UFUNCTION()
	void HandleCharacterDeath();

	UFUNCTION()
	virtual void OnTargetDeathHandle();

	void TryGetPatrolPath();

	virtual void OnPossess(APawn* _possPawn) override;

	virtual void OnUnPossess() override;

public:

	friend class AACFGroupAIController;


	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class AACFCharacter* GetBaseAICharacter() const { return CharacterOwned; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UACFCombatBehaviourComponent* GetCombatBehavior() const { return CombatBehaviorComponent; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UACFCommandsManagerComponent* GetCommandManager() const { return CommandsManagerComp; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UACFThreatManagerComponent* GetThreatManager() const { return ThreatComponent; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE float GetMaxDistanceFromLead() const { return MaxDistanceFromHome; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE float GetTeleportHomeDistanceTrigger() const { return TeleportHomeTriggerDistance; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE float GetLoseTargetDistance() const { return LoseTargetDistance; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE uint8 GetCommandDurationTimeKey()const { return commandDurationTimeKey; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE int32 GetIndexInGroup() const { return GroupIndex; }

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetDefaultState(EAIState inDefaultState)
	{
		DefaultState = inDefaultState; 
	}

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetWaitDurationTimeBK(float time);

	UFUNCTION(BlueprintPure, Category = ACF)
	float GetCommandDurationTimeBK() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	EAICombatState GetCombatStateBK() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	EAIState GetAIStateBK() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	AActor* GetTargetActorBK() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	int32 GetComboCounterBK() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	FVector GetTargetPointLocationBK() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	bool GetIsPausedBK() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	class AActor* GetLeadActorBK() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	float GetTargetActorDistanceBK() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	float GetTargetPointDistanceBK() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	float GetHomeDistanceBK() const;

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetTargetLocationBK(const FVector& targetLocation);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetLeadActorBK(AActor* leadActor);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetTargetActorBK(AActor* target);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetCurrentAIStateBK(EAIState aiState);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetComboCounterBK(int32 _comboCount);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetCombatStateBK(EAICombatState combatState);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetTargetPointDistanceBK(float distance);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetHomeDistanceBK(float distance);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetTargetActorDistanceBK(float distance);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetIsPausedBK(bool isPaused);
	
	UFUNCTION(BlueprintCallable, Category = ACF)
	float GetPathDistanceFromTarget();

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool TryGoToNextWaypoint();

	UFUNCTION(BlueprintCallable, Category = ACF)
	void TeleportNearLead();

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnAIStateChanged OnAIStateChanged;

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnAICombatStateChanged OnAICombatStateChanged;

	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetTarget(AActor* target);

	UFUNCTION(BlueprintPure, Category = ACF)
	AActor* GetTarget() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	bool HasTarget() const;

	void RequestAnotherTarget();

	void SetGroupOwner(class UACFGroupAIComponent* _Group, int _groupIndex, bool _disablePerception, bool bOverrideTeam);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void TriggerCommand(const FGameplayTag& command);

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UBehaviorTreeComponent* GetBehaviorThreeComponent() const { return BehaviorTreeComponent; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE  class UACFGroupAIComponent* GetGroup() const { return GroupOwner; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE bool ShouldCameHome() const { return bBoundToHome; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE bool ShouldTeleportHome() const{ return bTeleportHome; }


};
