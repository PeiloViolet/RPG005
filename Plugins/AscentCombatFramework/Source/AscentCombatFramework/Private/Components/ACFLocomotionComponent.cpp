// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Components/ACFLocomotionComponent.h"

#include <Animation/AnimEnums.h>
#include <Camera/CameraComponent.h>
#include <Components/ActorComponent.h>
#include <Engine/World.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/Controller.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/SpringArmComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include <TimerManager.h>

#include "ARSStatisticsComponent.h"
#include "Actors/ACFCharacter.h"
#include "Animation/ACFAnimInstance.h"
#include "CCMCameraFunctionLibrary.h"
#include "Components/ACFActionsManagerComponent.h"
#include "Game/ACFDamageType.h"
#include "Game/ACFFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

UACFLocomotionComponent::UACFLocomotionComponent()
{
    bEditableWhenInherited = true;
    PrimaryComponentTick.bCanEverTick = true;

    LocomotionStates.Add(FACFLocomotionState(ELocomotionState::EIdle, 0.f));
    LocomotionStates.Add(FACFLocomotionState(ELocomotionState::EWalk, 180.f));
    LocomotionStates.Add(FACFLocomotionState(ELocomotionState::EJog, 350.f));
    LocomotionStates.Add(FACFLocomotionState(ELocomotionState::ESprint, 550.f));

    SetIsReplicatedByDefault(true);
    currentLocomotionState = ELocomotionState::EIdle;
}

void UACFLocomotionComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UACFLocomotionComponent, targetLocomotionState);
    DOREPLIFETIME(UACFLocomotionComponent, reproductionType);
    DOREPLIFETIME(UACFLocomotionComponent, targetAlpha);
    DOREPLIFETIME(UACFLocomotionComponent, currentSnapConfiguration);
    DOREPLIFETIME(UACFLocomotionComponent, snapMultiplier);
    DOREPLIFETIME(UACFLocomotionComponent, aimOffest);
    DOREPLIFETIME(UACFLocomotionComponent, LocomotionStates);
    DOREPLIFETIME(UACFLocomotionComponent, CharacterMaxSpeed);
}

void UACFLocomotionComponent::BeginPlay()
{
    Super::BeginPlay();

    Character = Cast<AACFCharacter>(GetOwner());
    if (Character) {
        MovementComponent = Character->FindComponentByClass<UCharacterMovementComponent>();
        if (MovementComponent) {
            MovementComponent->RotationRate.Yaw = RotationRate;
        } else {
            UE_LOG(LogTemp, Warning, TEXT("Component not found in Character"));
        }
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Owner not found!"));
    }

    UCameraComponent* Camera = Character->FindComponentByClass<UCameraComponent>();
    USpringArmComponent* CameraBoom = Character->FindComponentByClass<USpringArmComponent>();

    if (Camera && CameraBoom) {
        Camera->bUsePawnControlRotation = false;
        CameraBoom->bUsePawnControlRotation = true;
    }

    SetStrafeMovement(bShouldStrafe);
    SetLocomotionState(DefaultState);

    UpdateCharacterMaxSpeed();

    LocomotionStates.Sort();
}

void UACFLocomotionComponent::TickComponent(
    float DeltaTime, enum ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateLocomotion();
    if (Character && bCanAim && Character->HasAuthority()) {
        UpdateAimOffset(DeltaTime);
    }
}

void UACFLocomotionComponent::UpdateAimOffset(float deltatime)
{
    FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(
        Character->GetControlRotation(), Character->GetActorRotation());

    float yawtarget = delta.Yaw;
    float pitchtarget = delta.Pitch;
    if (yawtarget >= 135.0f) {
        if (yawtarget < 90)
            yawtarget = -yawtarget;
        else
            yawtarget = yawtarget - 180.0f;

        pitchtarget = -pitchtarget;
    } else if (yawtarget <= -135.0f) {
        if (yawtarget > -90)
            yawtarget = -yawtarget;
        else
            yawtarget = yawtarget + 180.0f;

        pitchtarget = -pitchtarget;
    }

    aimOffest.AimOffsetYaw = FMath::FInterpTo(
        aimOffest.AimOffsetYaw, yawtarget, deltatime, AimOffsetSmoothingFactor);
    aimOffest.AimOffsetPitch = FMath::FInterpTo(aimOffest.AimOffsetPitch, pitchtarget, deltatime,
        AimOffsetSmoothingFactor);
}

void UACFLocomotionComponent::UpdateLocomotion()
{
    if (!Character) {
        return;
    }
    const UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();

    if (GetOwner() && MovementComp && !MovementComp->IsFalling() && reproductionType == EMontageReproductionType::ERootMotion) {
        if (MovementComp->GetGroundMovementMode() == MOVE_Walking) {
            for (int i = 0; i < LocomotionStates.Num() - 1; i++) {
                const float Speed = GetOwner()->GetVelocity().Size();
                if (FMath::IsNearlyZero(Speed) && currentLocomotionState != ELocomotionState::EIdle) {
                    HandleStateChanged(ELocomotionState::EIdle);
                }

                else if (LocomotionStates[i + 1].State != currentLocomotionState && Speed > LocomotionStates[i].MaxStateSpeed + 5.f && Speed <= LocomotionStates[i + 1].MaxStateSpeed + 5.f) {
                    HandleStateChanged(LocomotionStates[i + 1].State);
                }
            }

            if (currentLocomotionState == ELocomotionState::ESprint) {
                UACFAnimInstance* animInst = Character->GetACFAnimInstance();

                if (animInst) {
                    const float Direction = animInst->GetDirection();
                    if (FMath::Abs(Direction) > SprintDirectionCone) {
                        SetLocomotionState(ELocomotionState::EJog);
                    }
                }
            }
        } else {
            SetLocomotionState(ELocomotionState::EIdle);
        }
    }
}

void UACFLocomotionComponent::SetLocomotionState_Implementation(
    ELocomotionState State)
{
    FACFLocomotionState* _state = LocomotionStates.FindByKey(State);

    if (_state && MovementComponent) {
        targetLocomotionState = *(_state);
        MovementComponent->MaxWalkSpeed = GetCharacterMaxSpeedByState(State);
        targetLocomotionState.MaxStateSpeed = GetCharacterMaxSpeedByState(State);
        OnTargetLocomotionStateChanged.Broadcast(State);

        // OnLocomotionStateChanged.Broadcast(State);
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Locomotion State inexistent"));
    }
}

bool UACFLocomotionComponent::SetLocomotionState_Validate(
    ELocomotionState State)
{
    return true;
}

void UACFLocomotionComponent::SetLocomotionStateSpeed_Implementation(
    ELocomotionState State, float speed)
{
    LocomotionStates.AddUnique(FACFLocomotionState(State, speed));
    UpdateCharacterMaxSpeed();
}

bool UACFLocomotionComponent::SetLocomotionStateSpeed_Validate(
    ELocomotionState State, float speed)
{
    return true;
}

float UACFLocomotionComponent::GetCharacterMaxSpeedByState(
    ELocomotionState State)
{
    FACFLocomotionState* state = LocomotionStates.FindByKey(State);
    if (state) {
        return state->MaxStateSpeed;
    }
    return 0.0f;
}

void UACFLocomotionComponent::UpdateCharacterMaxSpeed()
{
    if (Character->HasAuthority()) {
		float maxspeed = 0.0f;
		for (const FACFLocomotionState& state : LocomotionStates) {
			if (state.MaxStateSpeed >= maxspeed) {
				maxspeed = state.MaxStateSpeed;
			}
		}
		CharacterMaxSpeed = maxspeed;
    }

}

void UACFLocomotionComponent::OnRep_LocomotionState()
{
    if (!MovementComponent) {
        Character = Cast<AACFCharacter>(GetOwner());
        if (Character) {
            MovementComponent = Character->FindComponentByClass<UCharacterMovementComponent>();
        }
    }
    MovementComponent->MaxWalkSpeed = GetCharacterMaxSpeedByState(targetLocomotionState.State);
    targetLocomotionState.MaxStateSpeed = GetCharacterMaxSpeedByState(targetLocomotionState.State);
}

void UACFLocomotionComponent::HandleStateChanged(ELocomotionState newState)
{
    if (currentLocomotionState == newState) {
        return;
    }

    FACFLocomotionState* oldState = LocomotionStates.FindByKey(currentLocomotionState);
    FACFLocomotionState* nextState = LocomotionStates.FindByKey(newState);

    if (oldState && nextState && Character) {
        if (GetOwner()->HasAuthority()) {
            UARSStatisticsComponent* statComp = Character->GetStatisticsComponent();

            statComp->RemoveAttributeSetModifier(oldState->StateModifier);
            statComp->AddAttributeSetModifier(nextState->StateModifier);
        }

        if (Character == UACFFunctionLibrary::GetLocalACFPlayerCharacter(this)) {
            UCCMCameraFunctionLibrary::StopCameraEvent(this,
                oldState->CameraMovement);
            UCCMCameraFunctionLibrary::TriggerCameraEvent(this,
                nextState->CameraMovement);
        }
    }
    currentLocomotionState = newState;
    OnLocomotionStateChanged.Broadcast(newState);
}

void UACFLocomotionComponent::ClientsShouldStrafe_Implementation(
    bool binShouldStrafe)
{
    if (Character) {
        bisStrafing = binShouldStrafe;
        MovementComponent->bOrientRotationToMovement = !bisStrafing; // Character moves in the direction of input..
        MovementComponent->bUseControllerDesiredRotation = bisStrafing;
    }
}

bool UACFLocomotionComponent::ClientsShouldStrafe_Validate(
    bool binShouldStrafe)
{
    return true;
}

void UACFLocomotionComponent::SetStrafeMovement_Implementation(
    bool shouldStrafe)
{
    if (Character) {
        bisStrafing = shouldStrafe;
        MovementComponent->bOrientRotationToMovement = !bisStrafing; // Character moves in the direction of input..
        MovementComponent->bUseControllerDesiredRotation = bisStrafing;
    }
    ClientsShouldStrafe(shouldStrafe);
}

bool UACFLocomotionComponent::SetStrafeMovement_Validate(bool shouldStrafe)
{
    return true;
}

void UACFLocomotionComponent::AccelerateToNextState_Implementation()
{
    LocomotionStates.Sort();

    const int32 actualindex = LocomotionStates.IndexOfByKey(currentLocomotionState);

    if (LocomotionStates.IsValidIndex(actualindex + 1)) {
        SetLocomotionState(LocomotionStates[actualindex + 1].State);
    }
}

bool UACFLocomotionComponent::AccelerateToNextState_Validate() { return true; }

void UACFLocomotionComponent::BrakeToPreviousState_Implementation()
{
    LocomotionStates.Sort();

    const int32 actualindex = LocomotionStates.IndexOfByKey(currentLocomotionState);

    if (LocomotionStates.IsValidIndex(actualindex - 1)) {
        SetLocomotionState(LocomotionStates[actualindex - 1].State);
    }
}

bool UACFLocomotionComponent::BrakeToPreviousState_Validate() { return true; }

// void UACFLocomotionComponent::TryToAim(bool bShouldAim)
// {
// 	if (!bShouldStrafe)
// 	{
// 		SetStrafeMovement(bShouldAim);
// 	}
// 	OnAimingStateChanged.Broadcast(bShouldAim);
// }

void UACFLocomotionComponent::TurnAtRate(float Rate)
{
    if (Character) {
        Character->AddControllerYawInput(Rate * TurnRate * GetWorld()->GetDeltaSeconds());
    }
}

void UACFLocomotionComponent::LookUpAtRate(float Rate)
{
    if (Character) {
        Character->AddControllerPitchInput(Rate * LookUpRate * GetWorld()->GetDeltaSeconds());
    }
}

void UACFLocomotionComponent::MoveForwardLocal(float Value)
{
    if (!bCanMove)
        return;
    // catch the forward axis

    MoveForwardAxis = Value;

    if (Character && Character->Controller && (MoveForwardAxis != 0.0f)) {
        // get forward vector
        const FVector Direction = Character->GetActorForwardVector();

        Character->AddMovementInput(Direction, Value);
    }
}

void UACFLocomotionComponent::TurnAtRateLocal(float Value)
{
    if (Value != 0 && Character) {
        const FRotator YawRotation(0, Value, 0);
        Character->AddActorLocalRotation(YawRotation);
    }
}

void UACFLocomotionComponent::MoveForward(float Value)
{
    if (!bCanMove)
        return;
    // catch the forward axis

    MoveForwardAxis = Value;

    if (Character && Character->Controller && (MoveForwardAxis != 0.0f)) {
        // find out which way is forward
        const FRotator Rotation = Character->Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        Character->AddMovementInput(Direction, Value);
    }
}

void UACFLocomotionComponent::MoveRight(float Value)
{
    if (!bCanMove)
        return;

    // Catch the right axis

    MoveRightAxis = Value;

    if (Character && Character->Controller && (MoveRightAxis != 0.0f)) {
        // find out which way is right
        const FRotator Rotation = Character->Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get right vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        // add movement in that direction
        Character->AddMovementInput(Direction, Value);
    }
}

FVector UACFLocomotionComponent::GetWorldMovementInputVector()
{
    if (Character) {
        const FVector localDir = FVector(MoveForwardAxis, MoveRightAxis, 0.f);

        FVector WorlDir = Character->GetActorForwardVector().Rotation().RotateVector(localDir);
        WorlDir.Normalize();
        return WorlDir;
    }
    return FVector();
}

FVector UACFLocomotionComponent::GetCameraMovementInputVector()
{
    if (Character) {
        const FVector localDir = FVector(MoveForwardAxis, MoveRightAxis, 0.f);
        FVector WorlDir = Character->GetController()->GetControlRotation().RotateVector(localDir);
        WorlDir.Normalize();
        return WorlDir;
    }
    return FVector();
}

void UACFLocomotionComponent::StartBlendingAdditiveAnimation()
{
    targetAlpha = 1.0f;
}

void UACFLocomotionComponent::StopBlendingAdditiveAnimation()
{
    targetAlpha = 0.0f;
}

float UACFLocomotionComponent::CalculateCurrentSnapMultiplier()
{
    if (Character && currentSnapConfiguration.target) {
        float distance = FVector::Distance(Character->GetActorLocation(),
            currentSnapConfiguration.target->GetActorLocation());

        if (distance <= currentSnapConfiguration.maxSnapMultiplier * currentSnapConfiguration.preferredDistance) {
            snapMultiplier = distance / currentSnapConfiguration.preferredDistance;
        }

        return snapMultiplier;
    }
    return 1.f;
}

FVector UACFLocomotionComponent::CalculateCurrentSnapDirection(
    float deltaTime)
{
    if (currentSnapConfiguration.target) {
        const FVector targetDir = currentSnapConfiguration.target->GetActorLocation() - Character->GetActorLocation();
        return FMath::VInterpTo(Character->GetActorForwardVector(), targetDir,
            MovementComponent->RotationRate.Yaw * currentSnapConfiguration.RotationRateSnapMult,
            deltaTime)
            .GetSafeNormal();
    }
    return Character->GetActorForwardVector();
}

void UACFLocomotionComponent::StartOverrideSpeedAndDirection(
    const FVector& direction)
{
    if (Character && Character->GetACFAnimInstance()) {
        Character->GetACFAnimInstance()->StartOverrideSpeedAnDirection(direction);
    }
}

void UACFLocomotionComponent::StopOverrideSpeedAndDirection()
{
    if (Character && Character->GetACFAnimInstance()) {
        Character->GetACFAnimInstance()->StopOverrideSpeedAnDirection();
    }
}

void UACFLocomotionComponent::SetMontageReprodutionType_Implementation(
    EMontageReproductionType repType)
{
    ClientsSetMontageReproduction(repType);
}

bool UACFLocomotionComponent::SetMontageReprodutionType_Validate(
    EMontageReproductionType repType)
{
    return true;
}

void UACFLocomotionComponent::ClientsSetMontageReproduction_Implementation(
    EMontageReproductionType repType)
{
    if (MovementComponent) {
        MovementComponent->RotationRate.Yaw = GetDefaultRotationRate();
    }
    reproductionType = repType;

    ERootMotionMode::Type rootMotionMode = ERootMotionMode::RootMotionFromMontagesOnly;

    switch (repType) {
    case EMontageReproductionType::ERootMotion:
    case EMontageReproductionType::ERootMotionScaled:

        rootMotionMode = ERootMotionMode::RootMotionFromMontagesOnly;
        break;
    case EMontageReproductionType::ECurveOverrideSpeed:
    case EMontageReproductionType::ECurveSnapsOnTarget:
    case EMontageReproductionType::ECurveOverrideSpeedAndDirection:
        rootMotionMode = ERootMotionMode::IgnoreRootMotion;
        break;
    default:
        break;
    }

    if (Character && Character->GetACFAnimInstance()) {
        Character->GetACFAnimInstance()->SetRootMotionMode(rootMotionMode);
    }
}

bool UACFLocomotionComponent::ClientsSetMontageReproduction_Validate(
    EMontageReproductionType repType)
{
    return true;
}

FVector UACFLocomotionComponent::GetCurrentInputVector()
{
    FVector dir = FVector(MoveForwardAxis, MoveRightAxis, 0.f);
    dir.Normalize();
    return dir;
}

EACFDirection UACFLocomotionComponent::GetCurrentInputDirection()
{
    if (Character) {
        const FVector direction = GetCurrentInputVector();

        if (FMath::Abs(direction.X) > FMath::Abs(direction.Y)) {
            if (direction.X > 0) {
                return EACFDirection::Front;
            } else {
                return EACFDirection::Back;
            }
        } else {
            if (direction.Y > 0) {
                return EACFDirection::Right;
            } else {
                return EACFDirection::Left;
            }
        }
    }
    return EACFDirection::Front;
}

void UACFLocomotionComponent::ResetToDefaultLocomotionState()
{
    SetLocomotionState(DefaultState);
}

void UACFLocomotionComponent::StartSnapping(
    const FSnapConfiguration& snapConfiguration)
{
    if (snapConfiguration.target && Character) {
        currentSnapConfiguration = snapConfiguration;
        SetMontageReprodutionType(EMontageReproductionType::ECurveSnapsOnTarget);
    }
}

void UACFLocomotionComponent::StopSnapping()
{
    currentSnapConfiguration = defaultSnapConfiguration;
    snapMultiplier = 1.0f;
}
