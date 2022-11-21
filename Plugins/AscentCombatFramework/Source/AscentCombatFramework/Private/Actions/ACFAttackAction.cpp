// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Actions/ACFAttackAction.h"
#include "Actions/ACFBaseAction.h"
#include "Actors/ACFCharacter.h"
#include "Animation/ACFAnimInstance.h"
#include "Components/ACFActionsManagerComponent.h"
#include "Components/ACFLocomotionComponent.h"
#include <GameFramework/CharacterMovementComponent.h>

UACFAttackAction::UACFAttackAction()
{
    AttackSnapConfig.maxSnapMultiplier = 2.5f;
    AttackSnapConfig.preferredDistance = 100.f;
    DamageToActivate = EDamageActivationType::EBoth;
}

void UACFAttackAction::OnActionStarted_Implementation()
{
    Super::OnActionStarted_Implementation();
    if (CharacterOwner && ActionConfig.MontageReproductionType == EMontageReproductionType::ECurveSnapsOnTarget)
    {
        UACFLocomotionComponent* locComp = CharacterOwner->GetLocomotionComponent();
        if (locComp && CharacterOwner->GetTarget()) {
            FSnapConfiguration SnapConfig;
            SnapConfig.maxSnapMultiplier = AttackSnapConfig.maxSnapMultiplier;
            SnapConfig.preferredDistance = AttackSnapConfig.preferredDistance;
            SnapConfig.target = CharacterOwner->GetTarget();
            locComp->StartSnapping(SnapConfig);
        }

        UCharacterMovementComponent* moveComp = CharacterOwner->GetCharacterMovement();

        if (moveComp)
        {
            oldRotationRate = moveComp->RotationRate.Yaw;
            moveComp->RotationRate.Yaw = oldRotationRate * RotationRateMultiplierDuringPreWinding;
        }
    }
   
    bIsEnterCombo = false;
}

void UACFAttackAction::OnActionEnded_Implementation()
{
    Super::OnActionEnded_Implementation();
    if (CharacterOwner && ActionsManager)
        {
        CharacterOwner->DeactivateDamage(DamageToActivate, TraceChannels);
    }

    if (CharacterOwner && ActionConfig.MontageReproductionType == EMontageReproductionType::ECurveSnapsOnTarget)
    {
        UACFLocomotionComponent* locComp = CharacterOwner->GetLocomotionComponent();
        if (locComp) {
            locComp->StopSnapping();
        }
        UCharacterMovementComponent* moveComp = CharacterOwner->GetCharacterMovement();
        if (moveComp) {
            moveComp->RotationRate.Yaw = oldRotationRate;
        }
    }
    if (!bIsEnterCombo)
    {
        CurrentComboIndex = 0;
    }
}

FName UACFAttackAction::GetMontageSectionName_Implementation()
{
    if (animMontage)
    {
        FName SectionName{NAME_None};
        switch (ComboIndexType)
        {
        case EComboIndexType::ESections:
            SectionName = animMontage->GetSectionName(CurrentComboIndex);
            break;
        case EComboIndexType::ESectionsSequence:
            if(ComboSequenceNames.IsValidIndex(CurrentComboIndex))
            SectionName = ComboSequenceNames[CurrentComboIndex];
            break;
        }
        if (SectionName != NAME_None)
        {
            return SectionName;
        }
        else
        {
            
            CurrentComboIndex = 0;
            return animMontage->GetSectionName(CurrentComboIndex);
        }
    }
    return NAME_None;
}

void UACFAttackAction::OnSubActionStateEntered_Implementation()
{
    Super::OnSubActionStateEntered_Implementation();

    if (CharacterOwner && ActionsManager)
    {
        CharacterOwner->ActivateDamage(DamageToActivate, TraceChannels);

        UCharacterMovementComponent* moveComp = CharacterOwner->GetCharacterMovement();

        if (moveComp) {
            moveComp->RotationRate.Yaw = oldRotationRate * RotationRateMultiplierDuringSwing;
        }
    }
}

void UACFAttackAction::OnSubActionStateExited_Implementation()
{
    Super::OnSubActionStateExited_Implementation();

    if (CharacterOwner && ActionsManager) {
        CharacterOwner->DeactivateDamage(DamageToActivate, TraceChannels);
    }

    UCharacterMovementComponent* moveComp = CharacterOwner->GetCharacterMovement();

    if (moveComp) {
        moveComp->RotationRate.Yaw = oldRotationRate * RotationRateMultiplierAfterWinding;
    }
}

void UACFAttackAction::OnActionTransition_Implementation(class UACFBaseAction* previousState)
{
    if (previousState && previousState->IsA(UACFAttackAction::StaticClass()))
    {
        bIsEnterCombo = true;
        switch (ComboCyclesType)
        {
        case EComboCycleType::ENor:
            CurrentComboIndex = 0;
            break;
        case EComboCycleType::EOverall:
            
            if(CurrentComboIndex >= EndComboIndex)
            {
                CurrentComboIndex = 0;
            }
            else
            {
                CurrentComboIndex++;
            }
            
            break;
        case  EComboCycleType::ELastCombo:
            
            if(CurrentComboIndex >= EndComboIndex)
            {
                CurrentComboIndex = StartCyclesComboIndex;
            }
            else
            {
                CurrentComboIndex++;
            }
            
            break;
        }
    }
    else
    {
        
        CurrentComboIndex = 0;
        bIsEnterCombo = false;
    }
}
