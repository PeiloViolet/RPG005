// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Actions/ACFBaseAction.h"
#include "ARSStatisticsComponent.h"
#include "Actors/ACFCharacter.h"
#include "Animation/ACFAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "Components/ACFLocomotionComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include "Game/ACFTypes.h"
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <TimerManager.h>
#include "AIController.h"

void UACFBaseAction::OnActionStarted_Implementation()
{
}

void UACFBaseAction::OnActionEnded_Implementation()
{
}

void UACFBaseAction::OnActionTransition_Implementation(class UACFBaseAction* previousState)
{
}

void UACFBaseAction::PlayEffects_Implementation()
{
    if (ActionConfig.ActionEffect.ActionParticle || ActionConfig.ActionEffect.NiagaraParticle || ActionConfig.ActionEffect.ActionSound) {
        UACFFunctionLibrary::PlayActionEffectLocally(ActionConfig.ActionEffect, CharacterOwner, this);
    }
}

void UACFBaseAction::OnTick_Implementation(float DeltaTime)
{
}

bool UACFBaseAction::CanExecuteAction_Implementation(class AACFCharacter* owner)
{
    return true;
}

FName UACFBaseAction::GetMontageSectionName_Implementation()
{
    return NAME_None;
}

void UACFBaseAction::OnNotablePointReached_Implementation()
{
}

void UACFBaseAction::OnSubActionStateEntered_Implementation()
{
}

void UACFBaseAction::OnSubActionStateExited_Implementation()
{
}
void UACFBaseAction::HandleMontageStarted(UAnimMontage* _animMontage)
{

    UAnimInstance* animinst = (CharacterOwner->GetMesh()->GetAnimInstance());
    if (animinst) {
        animinst->OnMontageStarted.RemoveDynamic(this, &UACFBaseAction::HandleMontageStarted);
        animinst->OnMontageBlendingOut.RemoveDynamic(this, &UACFBaseAction::HandleMontageFinished);
        if (_animMontage == animMontage) {
            animinst->OnMontageBlendingOut.AddDynamic(this, &UACFBaseAction::HandleMontageFinished);
        }
    }
}

void UACFBaseAction::ExecuteAction()
{
    FName Section;

    if (ActionConfig.bPlayRandomMontageSection)
    {
        const int32 numSections = animMontage->CompositeSections.Num();

        const int32 sectionToPlay = FMath::RandHelper(numSections);

        Section = animMontage->GetSectionName(sectionToPlay);
    }
    else
    {
        Section = GetMontageSectionName();
    }

    if (CharacterOwner)
        {
        UAnimInstance* animinst = (CharacterOwner->GetMesh()->GetAnimInstance());
        if (animinst)
        {
            animinst->OnMontageStarted.AddDynamic(this, &UACFBaseAction::HandleMontageStarted);
            UACFLocomotionComponent* locComp = CharacterOwner->GetLocomotionComponent();
            if (locComp)
            {
                locComp->SetMontageReprodutionType(ActionConfig.MontageReproductionType);
            }
            CharacterOwner->PlayNetworkedMontage(animMontage, GetPlayRate(), Section, ActionConfig.RootMotionScale);
            bIsExecutingAction = true;
        }
    }
}

void UACFBaseAction::ExitAction()
{
    if (ActionsManager) {
        ActionsManager->InternalExitAction();
    }
}

float UACFBaseAction::GetPlayRate_Implementation()
{
    return 1.f;
}

void UACFBaseAction::HandleMontageFinished(UAnimMontage* _animMontage, bool _bInterruptted)
{
    if (_animMontage == animMontage && bIsExecutingAction && !_bInterruptted && ActionConfig.bBindActionToAnimation) {
        ExitAction();
    }
}

void UACFBaseAction::Internal_OnActivated(class UACFActionsManagerComponent* _actionmanger, class UAnimMontage* _animMontage)
{
    ActionsManager = _actionmanger;
    animMontage = _animMontage;
    if (ActionsManager)
    {
        CharacterOwner = ActionsManager->CharacterOwner;
        ActionTag = ActionsManager->CurrentActionTag;
        StatisticComp = CharacterOwner->GetStatisticsComponent();

        if (StatisticComp)
        {
            StatisticComp->ConsumeStatistics(ActionConfig.ActionCost);
            StatisticComp->AddAttributeSetModifier(ActionConfig.AttributeModifier);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No Statistic Component!"));
            ExitAction();
            return;
        }
    }

    if (ActionConfig.bStopBehavioralThree)
    {
        const AAIController* contr = Cast<AAIController>(CharacterOwner->GetController());
        if (contr)
        {
            UBehaviorTreeComponent* behavComp = contr->FindComponentByClass<UBehaviorTreeComponent>();
            if (behavComp)
            {
                behavComp->PauseLogic("Blocking Action");
            }
        }
    }

    OnActionStarted();

    if (ActionConfig.bBindActionToAnimation)
    {
        
        ExecuteAction();
    }
}
void UACFBaseAction::Internal_OnDeactivated()
{

    if (bIsExecutingAction)
    {
        bIsExecutingAction = false;
    }

    UAnimInstance* animinst = (CharacterOwner->GetMesh()->GetAnimInstance());
    if (animinst)
    {
        animinst->OnMontageBlendingOut.RemoveDynamic(this, &UACFBaseAction::HandleMontageFinished);
    }
    UACFLocomotionComponent* locComp = CharacterOwner->GetLocomotionComponent();
    if (locComp)
    {
        //设置根运动模式模式
        locComp->SetMontageReprodutionType(EMontageReproductionType::ERootMotion);
    }

    if (StatisticComp)
    {
        StatisticComp->RemoveAttributeSetModifier(ActionConfig.AttributeModifier);
    }

    if (bIsInSubState)
    {
        OnSubActionStateExited();
    }

    if (ActionConfig.bStopBehavioralThree)
    {
        const AAIController* aiContr = Cast<AAIController>(CharacterOwner->GetController());
        if (aiContr) {
            UBehaviorTreeComponent* behavComp = aiContr->FindComponentByClass<UBehaviorTreeComponent>();
            if (behavComp) {
                behavComp->ResumeLogic("Blocking Action");
            }
        }
    }
    OnActionEnded();
}
float UACFBaseAction::GetCooldownTimeRemaining()
{
    UWorld* _world = GetWorld();
    if (_world) {
        return _world->GetTimerManager().GetTimerRemaining(CooldownTimerReference);

    } else {
        return 0.0f;
    }
}
