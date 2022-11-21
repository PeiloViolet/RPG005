// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Animation/ACFAnimInstance.h"
#include "Actors/ACFCharacter.h"
#include "Animation/ACFAnimset.h"
#include "Animation/AnimSequence.h"
#include "Components/ACFActionsManagerComponent.h"
#include "Components/ACFEffectsManagerComponent.h"
#include "Components/ACFLocomotionComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include <Animation/AimOffsetBlendSpace1D.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <TimerManager.h>

#include "AnimGraphRuntime/Public/KismetAnimationLibrary.h"


UACFAnimInstance::UACFAnimInstance()
{
}

void UACFAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    SetReferences();
    if(Movesets)
    {
        FACFMoveset* nextMov = Movesets->Movesets.FindByKey(UACFFunctionLibrary::GetMovesetTypeTagRoot());
        if (nextMov)
        {
            CurrentMoveset = *nextMov;
            bCanUseAdditive = CurrentMoveset.AdditiveBlendspace != nullptr ? CurrentMoveset.AdditiveBlendspace->IsValidLowLevel() : false;
        }
    }
    
    currentOverlayTag = UACFFunctionLibrary::GetAnimationOverlayTagRoot();
    SetAnimationOverlay(currentOverlayTag);
    bIsMoving = false;
}

void UACFAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (ACFCharacter && MovementComp && LocomotionComp) {

        Velocity = ACFCharacter->GetVelocity();
        Speed = Velocity.Size();
        NormalizedSpeed = Speed / LocomotionComp->GetCharacterMaxSpeed();
        Acceleration = MovementComp->GetCurrentAcceleration().Size();
        bIsAccelerating = Acceleration > 0;
        bIsMoving = !FMath::IsNearlyZero(NormalizedSpeed, IsMovingSpeedThreshold);

        Rotation = ACFCharacter->GetActorRotation();
        //Direction = CalculateDirection(Velocity, Rotation);
        Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Rotation);
        // Check if the play is falling?
        bIsInAir = MovementComp->IsFalling();

        AimOffsetPitch = LocomotionComp->GetAimOffset().AimOffsetPitch;
        AimOffsetYaw = LocomotionComp->GetAimOffset().AimOffsetYaw;

        UpdateAdditives(DeltaSeconds);
        UpdateTurningRate(DeltaSeconds);
        UpdateLeaning(DeltaSeconds);
        UpdateStopTransition(DeltaSeconds);

        switch (LocomotionComp->reproductionType) {
        case EMontageReproductionType::ECurveOverrideSpeed:
            OverrideSpeed();
            break;
        case EMontageReproductionType::ECurveSnapsOnTarget:
            Snap(DeltaSeconds);
            break;
        case EMontageReproductionType::ECurveOverrideSpeedAndDirection:
            OverrideSpeedAndDirection();
            break;
        default:
            break;
        }

        if (LocomotionComp->bForceSpeedToForward && !bIsInAir) {
            const FVector forward = ACFCharacter->GetActorForwardVector() * Speed;
            MovementComp->Velocity = FMath::VInterpTo(Velocity, forward, DeltaSeconds, LocomotionComp->SpeedToForwardInterpRate);
        }

		PreviousRotation = Rotation;
		PreviousSpeed = Speed;
    }
}

void UACFAnimInstance::UpdateAdditives(float DeltaSeconds)
{
    if (!FMath::IsNearlyEqual(CurrentAdditiveBlendingAlpha, LocomotionComp->targetAlpha, .001f)) {
        CurrentAdditiveBlendingAlpha = FMath::FInterpTo(CurrentAdditiveBlendingAlpha, LocomotionComp->targetAlpha, DeltaSeconds, CurrentMoveset.AdditiveBlendSpeed);
    }

    if (!CurrentOverlay.Animation || currentOverlayTag == FGameplayTag()) {
        CurrentOverlayBlendingAlpha = FMath::FInterpTo(CurrentOverlayBlendingAlpha, 0.f, DeltaSeconds, 1.f);
    } else if (CurrentOverlay.Animation && currentOverlayTag != FGameplayTag() && !FMath::IsNearlyEqual(CurrentOverlay.BlendingAlpha, CurrentOverlayBlendingAlpha, .001f)) {
        CurrentOverlayBlendingAlpha = FMath::FInterpTo(CurrentOverlayBlendingAlpha, CurrentOverlay.BlendingAlpha, DeltaSeconds, CurrentOverlay.InterpolationSpeed);
    }
}

void UACFAnimInstance::OverrideSpeedAndDirection()
{
    const FVector overrideVelocity = overrideDirection * GetCurveValue(SpeedCurveName) * AnimationCurvesScalingFactor;
    MovementComp->Velocity = FVector(overrideVelocity.X, overrideVelocity.Y, MovementComp->Velocity.Z);
}

void UACFAnimInstance::StartOverrideSpeedAnDirection(const FVector& direction)
{
    if (direction == FVector::ZeroVector) {
        overrideDirection = ACFCharacter->GetActorForwardVector();
    }

    overrideDirection = direction.GetSafeNormal();
    LocomotionComp->SetMontageReprodutionType(EMontageReproductionType::ECurveOverrideSpeedAndDirection);
}

void UACFAnimInstance::StopOverrideSpeedAnDirection()
{
    LocomotionComp->SetMontageReprodutionType(EMontageReproductionType::ERootMotion);
}

bool UACFAnimInstance::GetMovesetActionsByMoveset(const FGameplayTag& tag, FActionsMap& outActions)
{
    if(MovesetsActions)
    {
       
        FActionsMap* outMov = MovesetsActions->MovesetsActions.Find(tag);
        if (outMov)
        {
            
            outActions = *outMov;
            return true;
        }
    }
  
    return false;
}

bool UACFAnimInstance::GetAnimationOverlay(const FGameplayTag& tag, FAnimationOverlay& outOverlay)
{
    FAnimationOverlay* outOv = Overlays->Overlays.FindByKey(tag);
    if (outOv) {
        outOverlay = *outOv;
        return true;
    }

    return false;
}

void UACFAnimInstance::PlayFootstepFX() const
{
    if (ACFCharacter) {
        UACFEffectsManagerComponent* effectsMan = ACFCharacter->FindComponentByClass<UACFEffectsManagerComponent>();
        if (effectsMan) {
            effectsMan->TriggerFootstepFX();
        }
    }
}


void UACFAnimInstance::StartOverrideSpeed()
{
    LocomotionComp->SetMontageReprodutionType(EMontageReproductionType::ECurveOverrideSpeed);
}

void UACFAnimInstance::StopOverrideSpeed()
{
    LocomotionComp->SetMontageReprodutionType(EMontageReproductionType::ERootMotion);
}

void UACFAnimInstance::SetAnimationOverlay_Implementation(const FGameplayTag& overlayTag)
{
    if(Overlays && Overlays->Overlays.FindByKey(overlayTag))
    {
        FAnimationOverlay* overlay = Overlays->Overlays.FindByKey(overlayTag);
        if (overlay && overlay->Animation) {
            currentOverlayTag = overlayTag;
            CurrentOverlay.Animation = overlay->Animation;
            CurrentOverlay.BlendingAlpha = overlay->BlendingAlpha;
            CurrentOverlay.InterpolationSpeed = overlay->InterpolationSpeed;
        }
        else
        {
            RemoveOverlay();
        }
    }
    return;
}

FVector UACFAnimInstance::CalculateRelativeAccelerationAmount() const
{
    if (FVector::DotProduct(MovementComp->GetCurrentAcceleration(), Velocity) > 0.0f) {
        const float MaxAcc = ACFCharacter->GetCharacterMovement()->GetMaxAcceleration();
        return Rotation.UnrotateVector(
            MovementComp->GetCurrentAcceleration().GetClampedToMaxSize(MaxAcc) / MaxAcc);
    }

    const float MaxBrakingDec = ACFCharacter->GetCharacterMovement()->GetMaxBrakingDeceleration();
    return Rotation.UnrotateVector(
        MovementComp->GetCurrentAcceleration().GetClampedToMaxSize(MaxBrakingDec) / MaxBrakingDec);
}

void UACFAnimInstance::RemoveOverlay()
{
    currentOverlayTag = FGameplayTag();
}

void UACFAnimInstance::SelectStopAnim()
{
    if (CurrentMoveset.bUseStopTransition) {

        if (LocomotionComp->IsCharacterStrafing()) {
            const EACFDirection movedir = UACFFunctionLibrary::GetDirectionFromAngle(GetDirection());
            SetStopAnimByDirection(movedir);

        } else {
            SetStopAnimByDirection(EACFDirection::Front);
        }
    }
}

bool UACFAnimInstance::GetMovesetByTag(const FGameplayTag& movesetTag, FACFMoveset& outMoveset) const
{
	const FACFMoveset* outMov = Movesets->Movesets.FindByKey(movesetTag);
    if (outMov) {
        outMoveset = *outMov;
        return true;
    }
    return false;
}

bool UACFAnimInstance::GetOverlayByTag(const FGameplayTag& overlayTag, FAnimationOverlay& outMoveset) const
{
	const FAnimationOverlay* outMov = Overlays->Overlays.FindByKey(overlayTag);
	if (outMov) {
		outMoveset = *outMov;
		return true;
	}
	return false;
}


void UACFAnimInstance::SetOverlayBlendingAlpha(float blendingAlpha)
{
    CurrentOverlay.BlendingAlpha = blendingAlpha;
}

void UACFAnimInstance::SetStopAnimByDirection_Implementation(const EACFDirection movedir)
{
    bForceStopExit = false;
    StopAnim = nullptr;
    UAnimSequence* stopAnim = GetBestStopAnimation(movedir);
    if (stopAnim) {
        StopAnim = stopAnim;
    }
    else {
        bShouldStop = false;
        bForceStopExit = true;
    }
}

UAnimSequence* UACFAnimInstance::GetBestStopAnimation(const EACFDirection movedir)
{
	const ELocomotionState locState = LocomotionComp->GetCurrentLocomotionState();
	FStopAnimations* stopMove = CurrentMoveset.StopTransitions.Find(locState);

	//If we don't have a stop transition for this state, we try the previous
	if (!stopMove) {
		uint8 locIndex = (uint8)locState;
		while (!stopMove && locIndex > 0) {
			stopMove = CurrentMoveset.StopTransitions.Find((ELocomotionState)locIndex);
			locIndex--;
		}
	}

	if (!(stopMove && stopMove->StopAnimsMap.Contains(movedir))) {
		return nullptr;
	}
	return stopMove->StopAnimsMap.FindChecked(movedir);
}



void UACFAnimInstance::OverrideSpeed()
{
    const float speed = GetCurveValue(SpeedCurveName);
    MovementComp->Velocity = ACFCharacter->GetActorForwardVector() * speed * AnimationCurvesScalingFactor;
}

void UACFAnimInstance::FinishCurrentAction()
{
    if (ACFCharacter) {
        UACFActionsManagerComponent* amc = ACFCharacter->FindComponentByClass<UACFActionsManagerComponent>();
        if (amc) {
            amc->FreeAction();
        }
    }
}

void UACFAnimInstance::SetReferences()
{
    ACFCharacter = Cast<AACFCharacter>(TryGetPawnOwner());
    if (ACFCharacter) {
        MovementComp = ACFCharacter->GetCharacterMovement();
        LocomotionComp = ACFCharacter->GetLocomotionComponent();
    }
}

void UACFAnimInstance::UpdateTurningRate(float deltatime)
{
    const FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(Rotation, PreviousRotation);
    const float turn = delta.Yaw;
    TurnRate = FMath::FInterpTo(TurnRate, turn, deltatime, CurrentMoveset.TurnSmoothingRate);
    return;
}

void UACFAnimInstance::UpdateLeaning(float deltatime)
{
    if (CurrentMoveset.bUseLeaning) {
        const FRotator deltaRot = UKismetMathLibrary::NormalizedDeltaRotator(PreviousRotation, Rotation);
        const float normalizedDeltaYaw = deltaRot.Yaw / LocomotionComp->GetDefaultRotationRate();
        Leaning.X = (normalizedDeltaYaw / deltatime) * NormalizedSpeed;
        const float NormalizedDeltaSpeed = (Speed - PreviousSpeed) / MovementComp->GetMaxAcceleration();
        Leaning.Y = NormalizedDeltaSpeed / deltatime;
    }
}

void UACFAnimInstance::UpdateStopTransition(float deltatime)
{
    if (CurrentMoveset.bUseStopTransition) {
        bShouldStop = (PreviousSpeed - Speed) / deltatime >= CurrentMoveset.MinDecelerationToEnterStopTransition &&
             Speed > CurrentMoveset.MinSpeedToEnterStopTransition;
    }
    else {
        bShouldStop = false;
    }
}

void UACFAnimInstance::SetMoveset_Implementation(const FGameplayTag& Movetype)
{
    const FACFMoveset* nextMov = Movesets->Movesets.FindByKey(Movetype);
    UWorld* world = GetWorld();
    if (world && nextMov) {
        currentMovesetTag = Movetype;
        nextMoveset = *nextMov;
        bIsSwitchingMoveset = true;
        bCanUseAdditive = false;
        FTimerHandle TimerHandle;
        world->GetTimerManager().SetTimer(TimerHandle, this, &UACFAnimInstance::OnMovesetSwitchingFinished, MovesetSwitchTime, false);
    }
}

void UACFAnimInstance::OnMovesetSwitchingFinished()
{
    CurrentMoveset = nextMoveset;
    bCanUseAdditive = CurrentMoveset.AdditiveBlendspace->IsValidLowLevel();
    bIsSwitchingMoveset = false;
}

void UACFAnimInstance::Snap(float deltaTime)
{
    const float speed = GetCurveValue(SpeedCurveName);
    const FVector direction = LocomotionComp->CalculateCurrentSnapDirection(deltaTime);
    MovementComp->Velocity = direction * speed * LocomotionComp->CalculateCurrentSnapMultiplier() * AnimationCurvesScalingFactor;
}
