// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "Components/ACFActionsManagerComponent.h"
#include "CoreMinimal.h"
#include "Game/ACFPlayerController.h"
#include "Game/ACFTypes.h"
#include <GameplayTagContainer.h>
#include "Animation/AnimMontage.h"
#include "ACFBaseAction.generated.h"

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class ASCENTCOMBATFRAMEWORK_API UACFBaseAction : public UObject {
    GENERATED_BODY()

    friend UACFActionsManagerComponent;

public:
    /*Blueprint callable function that will get us the cooldown remaining for the action*/
    UFUNCTION(BlueprintCallable, Category = ACF)
    float GetCooldownTimeRemaining();

protected:
    /*Called when the action is successfully triggered*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnActionStarted();
    virtual void OnActionStarted_Implementation();

    /*Called when the action is finished. Normally if BindActionToAnimation is set to true
	this is called when the animation is finished*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnActionEnded();
    virtual void OnActionEnded_Implementation();

    /*Called when the action is successfully triggered and BEFORE the OnActionStarted.
	If the character is coming from the DefaultState previousState will be nullptr*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnActionTransition(class UACFBaseAction* previousState);
    virtual void OnActionTransition_Implementation(class UACFBaseAction* previousState);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void PlayEffects();
    virtual void PlayEffects_Implementation();

    /*Called every frame if the ActionsManagerComponent of this character has bCanTick set to true*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnTick(float DeltaTime);
    virtual void OnTick_Implementation(float DeltaTime);

    /*Used to implement your own activation condition for the execution of this action. */
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    bool CanExecuteAction(class AACFCharacter* owner);
    virtual bool CanExecuteAction_Implementation(class AACFCharacter* owner);

    /*Implement this to select the name of the montage section that should be played when executing this action */
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    FName GetMontageSectionName();
    virtual FName GetMontageSectionName_Implementation();

    /*To trigger a Notable point you have to add an ACFNotifyAction to your Animation Montage.
	Called during the execution of the action when the montage reaches the ACFNotifyAction*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnNotablePointReached();
    virtual void OnNotablePointReached_Implementation();

    /*To trigger a SubActionState you have to place a ACFActionSubState notify in your Animation Montage.
	Called during the execution of the action when the montage reaches the beginning of the Substate*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnSubActionStateEntered();
    virtual void OnSubActionStateEntered_Implementation();

    /*To trigger a SubActionState you have to place a ACFActionSubState notify in your Animation Montage.
	Called during the execution of the action when the montage reaches the end  of the Substate*/
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void OnSubActionStateExited();
    virtual void OnSubActionStateExited_Implementation();

    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    float GetPlayRate();
    virtual float GetPlayRate_Implementation();

    UFUNCTION(BlueprintCallable, Category = ACF)
    void ExecuteAction();

    /*Called to force the end of the action. Does not stop the animation but allows the trigger of eventually stored
	actions or set the actual state to the default one*/
    UFUNCTION(BlueprintCallable, Category = ACF)
    void ExitAction();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    FActionConfig ActionConfig;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    class UACFActionsManagerComponent* ActionsManager;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    class AACFCharacter* CharacterOwner;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    class UAnimMontage* animMontage;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FGameplayTag ActionTag;

    virtual void Internal_OnActivated(class UACFActionsManagerComponent* _actionmanger, class UAnimMontage* _animMontage);

    virtual void Internal_OnDeactivated();

    UWorld* GetWorld() const override { return CharacterOwner ? CharacterOwner->GetWorld() : nullptr; }

    /*A reference to the cooldown started for the Action*/
    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FTimerHandle CooldownTimerReference;

private:
    UFUNCTION()
    void HandleMontageFinished(UAnimMontage* _animMontage, bool _bInterruptted);

    UFUNCTION()
    void HandleMontageStarted(UAnimMontage* _animMontage);

    bool bIsExecutingAction = false;

    bool bIsInSubState = false;

    UPROPERTY()
    class UARSStatisticsComponent* StatisticComp;
};
