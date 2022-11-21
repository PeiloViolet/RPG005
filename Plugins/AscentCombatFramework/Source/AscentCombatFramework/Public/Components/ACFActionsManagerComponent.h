// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include <Engine/DataTable.h>
 #include <GameplayTagContainer.h>
#include "Components/ACFEquipmentComponent.h"
#include "ARSTypes.h"
#include "ARSStatisticsComponent.h"
#include "Game/ACFTypes.h"
#include "Animation/ACFAnimset.h"
#include "ACFActionsManagerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionStarted, FGameplayTag, ActionState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionEnded, FGameplayTag, ActionState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionTriggered, FGameplayTag, ActionState, EActionPriority, Priority);




UCLASS(ClassGroup=(ACF), Blueprintable ,meta=(BlueprintSpawnableComponent) )
class ASCENTCOMBATFRAMEWORK_API UACFActionsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFActionsManagerComponent();

protected:

	friend class UACFBaseAction;
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	class AACFCharacter* CharacterOwner; 
	 
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = ACF)
	bool bCanTick = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = ACF)
	bool bPrintDebugInfo = false;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = ACF)
	void TriggerActionByName(FName ActionTagName, EActionPriority Priority = EActionPriority::ELow);

	UFUNCTION(BlueprintCallable, Category = ACF)
	 void LockActionsTrigger() {
	 bIsLocked = true;
	 TerminateCurrentAction();
	 }

	UFUNCTION(BlueprintCallable, Category = ACF)
	void UnlockActionsTrigger() { bIsLocked = false; }

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = ACF)
	void TriggerAction(FGameplayTag ActionState, EActionPriority Priority = EActionPriority::ELow);

	UFUNCTION(BlueprintCallable, Category = ACF)
	 void StopStoringActions() {
		bCanStoreAction = false;
	}

	UFUNCTION(BlueprintCallable, Category = ACF)
		void StartStoringActions() {
		bCanStoreAction = true;
	}

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool IsActionOnCooldown(FGameplayTag action) const;

	UFUNCTION(BlueprintCallable, Category = ACF)
	void StoreAction(FGameplayTag Action);

	UFUNCTION(Blueprintpure, Category = ACF)
	FORCEINLINE FGameplayTag GetStoredAction() const { return StoredAction; }

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool CanExecuteAction(FGameplayTag Action) const ;

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnActionStarted OnActionStarted;

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnActionEnded OnActionFinished;

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnActionTriggered OnActionTriggered;

	UFUNCTION(BlueprintCallable, Category = ACF)
	void ExitAction();

	UFUNCTION(BlueprintPure, Category = ACF)
	FGameplayTag GetCurrentActionTag() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UACFBaseAction* GetCurrentAction() const {
		return PerformingAction;
	}

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool GetActionByTag(const FGameplayTag& Action, FActionState& outAction) const;

	UFUNCTION(BlueprintCallable, Category = ACF)
	void PlayCurrentActionFX();

	UFUNCTION(BlueprintCallable, Category = ACF)
	void PlayCurrentActionFXLocally();

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE bool IsInActionState(FGameplayTag state) const { return CurrentActionTag == state; }
	
	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE bool IsCharacterPerformingAction() const { return bIsPerformingAction; }

	void AnimationsReachedNotablePoint();

	void StartSubState();
	void EndSubState();
	UFUNCTION(BlueprintCallable, Category = ACF)
	void FreeAction();
private:

	void InternalExitAction();
	
	void LaunchAction(const FGameplayTag& ActionState, const EActionPriority priority);

	void SetCurrentAction(const FGameplayTag& state);
	UFUNCTION(BlueprintCallable, Category = ACF)
	void TerminateCurrentAction();

	UPROPERTY()
	class UACFAnimInstance* animInst;

// 	UFUNCTION()
// 	void HandleMovementTypeChanged(FGameplayTag Movetype);

	UFUNCTION(NetMulticast, WithValidation, Reliable)
	void ClientsReceiveActionStarted(const FGameplayTag& ActionState);

	UFUNCTION(NetMulticast, WithValidation, Reliable)
	void ClientsReceiveActionEnded(const FGameplayTag& ActionState);

	UPROPERTY()
	bool bIsPerformingAction;

	UPROPERTY()
	class UACFBaseAction* PerformingAction;

	UPROPERTY()
	FActionState CurrentActionState;

	UPROPERTY()
	FGameplayTag CurrentActionTag;

	UPROPERTY()
	FGameplayTag StoredAction;

	UPROPERTY()
	int32 CurrentPriority;

	UPROPERTY()
	bool bCanStoreAction = true;

	UPROPERTY()
	class UARSStatisticsComponent* StatisticComp;

	UPROPERTY()
	FGameplayTag DefaultActionState;

	void PrintStateDebugInfo(bool bIsEntring);

	UPROPERTY()
	TArray<FGameplayTag> _onCooldownActions;

	void StartCooldown(const FGameplayTag& action, FActionState& actionState);

	UFUNCTION()
	void OnCooldownFinished(const FGameplayTag& action);

	UPROPERTY()
	bool bIsLocked = false;
};
