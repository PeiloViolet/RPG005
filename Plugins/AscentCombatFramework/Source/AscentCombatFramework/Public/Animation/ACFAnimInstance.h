// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ACFAnimset.h"
#include "Actions/ACFBaseAction.h"
#include "Animation/AnimInstance.h"
#include "Game/ACFTypes.h"
#include <GameplayTagContainer.h>

#include "RPGCommonActionsData.h"
#include "RPGMovesetsActionsData.h"
#include "RPGMovesetsData.h"
#include "RPGOverLaysData.h"
#include "ACFAnimInstance.generated.h"

/**
 * 
 */

UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFAnimInstance : public UAnimInstance {
    GENERATED_BODY()

public:
    UACFAnimInstance();
    friend class AACFCharacter;
    friend class UACFLocomotionComponent;
    friend class UACFActionsManagerComponent;
    /* begin play */
    virtual void NativeInitializeAnimation() override;
    /* tick */
    virtual void NativeUpdateAnimation(float DeltaTime) override;

    void UpdateAdditives(float DeltaSeconds);

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetDirection() { return Direction; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetNormalizedSpeed() const { return NormalizedSpeed; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE float GetTurnRate() const { return TurnRate; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool GetMovesetActionsByMoveset(const FGameplayTag& tag, FActionsMap& outActions);

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool GetAnimationOverlay(const FGameplayTag& tag, FAnimationOverlay& outOverlay);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void PlayFootstepFX() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = ACF)
    void SetAnimationOverlay(const FGameplayTag& overlayTag);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetMovesetActionsTag(const FGameplayTag& actionsTag) {
        currentMovesetActionsTag = actionsTag;
    }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = ACF)
    void SetMoveset(const FGameplayTag& Movetype);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void RemoveOverlay();

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SelectStopAnim();

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool IsVelocityOverrideByAnim() const { return bVelocityOverrideByAnim; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FACFMoveset GetCurrentMoveset() const { return CurrentMoveset; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool GetMovesetByTag(const FGameplayTag& movesetTag, FACFMoveset& outMoveset) const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool GetOverlayByTag(const FGameplayTag& overlayTag, FAnimationOverlay& outMoveset) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FGameplayTag GetCurrentMovesetTag() const { return currentMovesetTag; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FGameplayTag GetCurrentMovesetActionsTag() const { return currentMovesetActionsTag; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE FGameplayTag GetCurrentOverlay() const { return currentOverlayTag; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetOverlayBlendingAlpha(float blendingAlpha);

protected:
    UFUNCTION(BlueprintCallable, Category = ACF)
    void FinishCurrentAction();

    // ----- READ ONLY VARIABLES ---- //
    UPROPERTY(BlueprintReadWrite, Category = ACF)
    float AimOffsetYaw;

    UPROPERTY(BlueprintReadWrite, Category = ACF)
    float AimOffsetPitch;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    float TurnRate;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FACFMoveset nextMoveset;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FGameplayTag currentMovesetActionsTag;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FGameplayTag currentMovesetTag;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FGameplayTag currentOverlayTag;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bCanUseAdditive = false;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FACFMoveset CurrentMoveset;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FAnimationOverlay CurrentOverlay;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bIsSwitchingMoveset = false;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    float Speed;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    float NormalizedSpeed;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    float Acceleration;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FVector Velocity;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FRotator Rotation;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bIsInAir;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bIsMoving;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bIsAccelerating;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bVelocityOverrideByAnim;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    float CurrentAdditiveBlendingAlpha = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    float CurrentOverlayBlendingAlpha = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    float Direction;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bIsBlendingPose = false;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bShouldStop = false;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bForceStopExit = false;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    float PreviousSpeed;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    UAnimSequence* StopAnim;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    FVector2D Leaning;

    // ----- END READ ONLY VARIABLES ---- //

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF | Config")
    float AnimationCurvesScalingFactor = 1.f;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF | Config")
    FName SpeedCurveName = "Speed";

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ACF | Config")
    float MovesetSwitchTime = 0.7f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Config")
    float IsMovingSpeedThreshold = 0.02f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPGActions")
    TObjectPtr<URPGCommonActionsData> CommonActionsData;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPGActions")
    TObjectPtr<URPGMovesetsActionsData> MovesetsActions;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPGActions")
    TObjectPtr<URPGMovesetsData> Movesets;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPGActions")
    TObjectPtr<URPGOverLaysData> Overlays;
/*
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Actions")
    TMap<FGameplayTag, FActionState> CommonActions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Actions")
    TMap<FGameplayTag, FActionsMap> MovesetsActions;

    UPROPERTY(EditAnywhere, meta = (TitleProperty = "TagName"), BlueprintReadWrite, Category = "ACF | Movesets")
    TArray<FACFMoveset> Movesets;

    UPROPERTY(EditAnywhere, meta = (TitleProperty = "TagName"), BlueprintReadWrite, Category = "ACF | Movesets")
    TArray<FAnimationOverlay> Overlays;
*/
    
    UFUNCTION(BlueprintNativeEvent, Category = ACF)
    void SetStopAnimByDirection(const EACFDirection movedir);
    virtual void SetStopAnimByDirection_Implementation(const EACFDirection movedir);

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    class AACFCharacter* ACFCharacter;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    class UCharacterMovementComponent* MovementComp;

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    class UACFLocomotionComponent* LocomotionComp;

private:
    bool bOverrideVelocitybyVector = false;

    FVector overrideDirection;

    void Snap(float deltaTime);
    void SetReferences();
    void OverrideSpeedAndDirection();

    void OverrideSpeed();

    void UpdateTurningRate(float deltatime);
    void UpdateLeaning(float deltatime);
    void UpdateStopTransition(float deltatime);

    FVector CalculateRelativeAccelerationAmount() const;
    FRotator PreviousRotation;

    void OnMovesetSwitchingFinished();

    void StartOverrideSpeed();

    void StopOverrideSpeed();

    void StartOverrideSpeedAnDirection(const FVector& direction);

    void StopOverrideSpeedAnDirection();

    UAnimSequence* GetBestStopAnimation(const EACFDirection movedir);
};
