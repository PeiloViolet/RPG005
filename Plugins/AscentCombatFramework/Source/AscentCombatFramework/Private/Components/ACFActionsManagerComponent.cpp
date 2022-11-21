// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Components/ACFActionsManagerComponent.h"
#include "Actors/ACFCharacter.h"
#include "Actions/ACFBaseAction.h"
#include <GameplayTagsManager.h>
#include "Game/ACFFunctionLibrary.h"
#include <Engine/Engine.h>
#include "Animation/ACFAnimset.h"
#include <Kismet/KismetSystemLibrary.h>
#include "ARSStatisticsComponent.h"
#include "ARSTypes.h"
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <TimerManager.h>
#include <Engine/World.h>
#include "Animation/ACFAnimInstance.h"


// Sets default values for this component's properties
UACFActionsManagerComponent::UACFActionsManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	SetComponentTickEnabled(false);
	// ...
}


// Called when the game starts
void UACFActionsManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	StoredAction = DefaultActionState;
	CharacterOwner = Cast<AACFCharacter>(GetOwner());
	if (CharacterOwner)
	{	
		animInst = CharacterOwner->GetACFAnimInstance();
		StatisticComp = CharacterOwner->GetStatisticsComponent();

		if (!StatisticComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("No Statistiscs Component - ActionsManager"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid ACF Character - ActionsManager"));
	}
	SetComponentTickEnabled(bCanTick); 
	DefaultActionState = UACFFunctionLibrary::GetDefaultActionsState();
	StoredAction = DefaultActionState;
	CurrentActionTag = DefaultActionState;

}


// Called every frame
void UACFActionsManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsPerformingAction && PerformingAction)
	{
		PerformingAction->OnTick(DeltaTime);
	}
}

void UACFActionsManagerComponent::TriggerActionByName(FName ActionTagName, EActionPriority Priority /*= EActionPriority::ELow*/)
{
	FGameplayTag _tag = UGameplayTagsManager::Get().RequestGameplayTag(ActionTagName);

	if (_tag.IsValid()) {
		TriggerAction(_tag, Priority);
	}
}

void UACFActionsManagerComponent::TriggerAction_Implementation(FGameplayTag ActionState, EActionPriority Priority)
{
	if (!CharacterOwner) return;

	OnActionTriggered.Broadcast(ActionState, Priority);
	
	FActionState _action;
	
	
	if (GetActionByTag(ActionState, _action) && _action.Action && CanExecuteAction(ActionState))
	{
		//判断优先级
		if ((((int32)Priority > CurrentPriority) ) || Priority == EActionPriority::EHighest)
		{
			
			LaunchAction(ActionState, Priority);		
		}
		else if (CurrentActionTag != FGameplayTag() && bCanStoreAction && _action.Action->ActionConfig.bCanBeStored)
		{
			
			StoreAction(ActionState);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Action Configuration (%s) - ActionsManager"), *ActionState.ToString());
	}
}

bool UACFActionsManagerComponent::TriggerAction_Validate(FGameplayTag ActionState, EActionPriority Priority /*= EActionPriority::ELow*/)
{
	return true;
}

bool UACFActionsManagerComponent::IsActionOnCooldown(FGameplayTag action) const
{
	return _onCooldownActions.Contains(action);
}

void UACFActionsManagerComponent::StoreAction(FGameplayTag ActionState)
{
	StoredAction = ActionState;
}

void UACFActionsManagerComponent::LaunchAction(const FGameplayTag& ActionState, const EActionPriority priority)
{
	FActionState _action;
	
	if (GetActionByTag(ActionState, _action) && _action.Action)
	{
		//如果当前有Action
		if (PerformingAction)
		{
			_action.Action->OnActionTransition(PerformingAction);
			//终止当前Action
			TerminateCurrentAction();		
		}
		PerformingAction = _action.Action;
		bIsPerformingAction = true;
		
		PerformingAction->Internal_OnActivated(this, _action.MontageAction);
		ClientsReceiveActionStarted(ActionState);
		CurrentPriority = (int32)priority;	
		if (PerformingAction->ActionConfig.bPlayEffectOnActionStart)
		{
			PerformingAction->PlayEffects();
		}
	}
}


void UACFActionsManagerComponent::SetCurrentAction(const FGameplayTag& ActionState )
{
	CurrentActionTag =  ActionState;
}

//终止当前Action
void UACFActionsManagerComponent::TerminateCurrentAction()
{
	if (bIsPerformingAction && PerformingAction)
	{
		//停止Action时调用
		PerformingAction->Internal_OnDeactivated();
		ClientsReceiveActionEnded(CurrentActionTag);
		CurrentPriority = -1;
	}
	bIsPerformingAction = false; 

}

void UACFActionsManagerComponent::ClientsReceiveActionEnded_Implementation(const FGameplayTag& ActionState)
{

	PrintStateDebugInfo(false);
	OnActionFinished.Broadcast(ActionState);
}

bool UACFActionsManagerComponent::ClientsReceiveActionEnded_Validate(const FGameplayTag& ActionState)
{
	return true;
}

void UACFActionsManagerComponent::ClientsReceiveActionStarted_Implementation(const FGameplayTag& ActionState)
{
	SetCurrentAction(ActionState);
	OnActionStarted.Broadcast(ActionState);
	PrintStateDebugInfo(true);

	FActionState _action;		
	if (GetActionByTag(ActionState, _action) && _action.Action)
	{
		StartCooldown(ActionState, _action);
		_action.Action->CharacterOwner = CharacterOwner;

	}


}

bool UACFActionsManagerComponent::ClientsReceiveActionStarted_Validate(const FGameplayTag& ActionState)
{
	return true;
}

bool UACFActionsManagerComponent::CanExecuteAction(FGameplayTag ActionState ) const
{
	FActionState _action;
	
	
	if (GetActionByTag(ActionState, _action) && _action.Action && StatisticComp)
	{
		if (CharacterOwner->GetCharacterMovement()->IsFalling() && !_action.Action->ActionConfig.bPerformableInAir)
		{
			return false;
		}
		if (StatisticComp->CheckCosts(_action.Action->ActionConfig.ActionCost) && StatisticComp->CheckAttributesRequirements(_action.Action->ActionConfig.Requirements)
			&& !IsActionOnCooldown(ActionState) && !bIsLocked && _action.Action->CanExecuteAction(CharacterOwner))
		{
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Actions Conditions are not verified"));
	}
	return false;
}

void UACFActionsManagerComponent::InternalExitAction()
{   
	if (bIsPerformingAction && PerformingAction)
	{
		TerminateCurrentAction();

		if (StoredAction != DefaultActionState )
		{
			TriggerAction(StoredAction);
			StoreAction(DefaultActionState);
		}
		else 
		{
			SetCurrentAction(DefaultActionState);
			ClientsReceiveActionStarted(DefaultActionState);
			
			PerformingAction = nullptr;
		}
	}
}

void UACFActionsManagerComponent::ExitAction()
{
	InternalExitAction();
}

void UACFActionsManagerComponent::FreeAction()
{
// 	bExitedPreventively = true;
// 	InternalExitAction();
	CurrentPriority = -1;
	
	if (StoredAction != DefaultActionState)
	{
		TriggerAction(StoredAction, EActionPriority::ELow);
		StoreAction(DefaultActionState);
	}
}

FGameplayTag UACFActionsManagerComponent::GetCurrentActionTag() const
{
	return CurrentActionTag;
}


bool UACFActionsManagerComponent::GetActionByTag(const FGameplayTag& Action, FActionState& outAction) const
{
	if (!animInst) return false;
	//获取MovesetAction
	FActionsMap movset;  
	animInst->GetMovesetActionsByMoveset(animInst->GetCurrentMovesetActionsTag(), movset);
	//查找MovesetAction
	FActionState* actionState = movset.Actions.Find(Action);
	if (actionState)
	{
		outAction = *actionState;
		//如果找到Action，则返回;
		return true;
	}
	
	//查找CommonActions
	actionState = animInst->CommonActionsData->CommonActions.Find(Action);
	if (actionState)
	{
		outAction = *actionState;
		return true;
	}
	
	return false;
}

void UACFActionsManagerComponent::PlayCurrentActionFX()
{
	if (PerformingAction) {
		PerformingAction->PlayEffects();
	}
}

void UACFActionsManagerComponent::PlayCurrentActionFXLocally()
{
}

void UACFActionsManagerComponent::AnimationsReachedNotablePoint()
{
	if (bIsPerformingAction && PerformingAction && PerformingAction->bIsExecutingAction && CharacterOwner && CharacterOwner->HasAuthority())
	{
		PerformingAction->OnNotablePointReached();
	}
}

void UACFActionsManagerComponent::StartSubState()
{
	if (bIsPerformingAction && PerformingAction && PerformingAction->bIsExecutingAction  && CharacterOwner && CharacterOwner->HasAuthority())
	{
		PerformingAction->bIsInSubState = true;
		PerformingAction->OnSubActionStateEntered();
	}
}

void UACFActionsManagerComponent::EndSubState()
{
	if (PerformingAction && PerformingAction->bIsExecutingAction && CharacterOwner && CharacterOwner->HasAuthority())
	{
		PerformingAction->bIsInSubState = false;
		PerformingAction->OnSubActionStateExited();
	}
}

void UACFActionsManagerComponent::PrintStateDebugInfo(bool bIsEntring)
{
	if (bPrintDebugInfo && GEngine && CharacterOwner)
	{
		FString ActionName;
		CurrentActionTag.GetTagName().ToString(ActionName);
		FString MessageToPrint;
		if (bIsEntring)
		{
			MessageToPrint = CharacterOwner->GetName() + FString(" Entered State:") + ActionName;
		}
		else
		{
			MessageToPrint = CharacterOwner->GetName() + FString(" Exited State:") + ActionName;
		}

		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, MessageToPrint, false);

	}
}

void UACFActionsManagerComponent::StartCooldown(const FGameplayTag& action, FActionState& actionState)
{
	if (actionState.Action->ActionConfig.CoolDownTime == 0.f)
		return;

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;
	TimerDel.BindUFunction(this, FName("OnCooldownFinished"), action);

	UWorld* _world = GetWorld();
	if (_world)
	{
		_onCooldownActions.Add(action);
		_world->GetTimerManager().SetTimer(TimerHandle, TimerDel, actionState.Action->ActionConfig.CoolDownTime, false);
		actionState.Action->CooldownTimerReference = TimerHandle;
	}
}

void UACFActionsManagerComponent::OnCooldownFinished(const FGameplayTag& action)
{
	if (_onCooldownActions.Contains(action)) {
		_onCooldownActions.Remove(action);
	}
}


