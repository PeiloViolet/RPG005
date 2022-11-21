// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "CASAnimMasterComponent.h"
#include "CASAnimSlaveComponent.h"
#include "CASSnapperComponent.h"
#include "CASTypes.h"
#include "GameFramework/Character.h"
#include <Animation/AnimInstance.h>
#include <Kismet/KismetMathLibrary.h>

// Sets default values for this component's properties
UCASAnimMasterComponent::UCASAnimMasterComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);

    // ...
}

bool UCASAnimMasterComponent::TryPlayCombinedAnimation(ACharacter* otherCharachter, const FGameplayTag& combineAnimTag)
{
    if (!CanPlayCombinedAnimWithCharacter(otherCharachter, combineAnimTag)) {
        return false;
    }

    PlayCombinedAnimation(otherCharachter, combineAnimTag);
    return true;
}

void UCASAnimMasterComponent::PlayCombinedAnimation_Implementation(class ACharacter* otherCharachter, const FGameplayTag& combineAnimTag)
{
    const FCombinedAnimsMaster* animConfig = GetCombinedAnimTag(combineAnimTag);

    UCASSnapperComponent* snapComponent = characterOwner->FindComponentByClass<UCASSnapperComponent>();
    if (animConfig && animConfig->MasterAnimMontage && snapComponent) {
        currentAnim = FCurrentCombinedAnim(*animConfig, combineAnimTag, otherCharachter);
        if (characterOwner->GetDistanceTo(otherCharachter) >= animConfig->MaxDistanceToStartCombinedAnimation) {
            snapComponent->OnSnapPointReached.AddDynamic(this, &UCASAnimMasterComponent::HandleSnapPointReached);
            snapComponent->SnapToActor(otherCharachter, SnapSpeed, animConfig->MaxDistanceToStartCombinedAnimation, SnapTimeout);
        }

        StartAnim();

        bIsPlayingCombAnim = true;
        return;
    }
}

bool UCASAnimMasterComponent::PlayCombinedAnimation_Validate(class ACharacter* otherCharachter, const FGameplayTag& combineAnimTag)
{
    return true;
}

bool UCASAnimMasterComponent::CanPlayCombinedAnimWithCharacter(ACharacter* otherCharachter, const FGameplayTag& combineAnimTag)
{
    if (bIsPlayingCombAnim || !otherCharachter || !IsValid(otherCharachter)) {
        return false;
    }

    if (combineAnimTag == FGameplayTag()) {
        return false;
    }

    if (!MasterAnimsConfig) {
        UE_LOG(LogTemp, Warning, TEXT("NO Anim Data Table! -UCASAnimMasterComponent::TryPlayCombinedAnimation "));
        return false;
    }

    FCombinedAnimsMaster* animConfig = GetCombinedAnimTag(combineAnimTag);

    UCASAnimSlaveComponent* slaveComp = otherCharachter->FindComponentByClass<UCASAnimSlaveComponent>();
    if (!slaveComp || !animConfig) {
        return false;
    }

    if (!slaveComp->CanStartCombinedAnimation(combineAnimTag, GetCharacterOwner()) || !EvaluateCombinedAnim(*animConfig, otherCharachter)) {
        return false;
    }

    UCASSnapperComponent* snapComponent = characterOwner->FindComponentByClass<UCASSnapperComponent>();
    return (animConfig->MasterAnimMontage && snapComponent);
}

void UCASAnimMasterComponent::MulticastPlayAnimMontage_Implementation(class UAnimMontage* AnimMontage, class ACharacter* character)
{
    if (!character) {
        return;
    }
    UAnimInstance* animinst = character->GetMesh()->GetAnimInstance();
    if (animinst) {
        animinst->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
        character->PlayAnimMontage(AnimMontage);
    }
}

bool UCASAnimMasterComponent::MulticastPlayAnimMontage_Validate(class UAnimMontage* AnimMontage, class ACharacter* character)
{
    return true;
}



FCombinedAnimsMaster* UCASAnimMasterComponent::GetCombinedAnimTag(const FGameplayTag& combineAnimTag)
{
    const auto anims = MasterAnimsConfig->GetRowMap();
    for (const auto anim : anims) {
        FCombinedAnimsMaster* currConfig = (FCombinedAnimsMaster*)anim.Value;
        if (currConfig && currConfig->AnimTag == combineAnimTag) {
            return currConfig;
        }
    }
    return nullptr;
}

// Called when the game starts
void UCASAnimMasterComponent::BeginPlay()
{
    Super::BeginPlay();

    characterOwner = Cast<ACharacter>(GetOwner());
}

bool UCASAnimMasterComponent::EvaluateCombinedAnim(const FCombinedAnimsMaster& animConfig, const ACharacter* otherChar) const
{
    return animConfig.MasterAnimMontage != nullptr && characterOwner->GetDistanceTo(otherChar) <= animConfig.MaxDistanceToStartSnapping && (animConfig.SlaveRequiredDirection == GetCharacterRelativedDirection(otherChar) || animConfig.SlaveRequiredDirection == ERelativeDirection::EAny);
}

void UCASAnimMasterComponent::OnCombinedAnimStarted(const FGameplayTag& animTag)
{
}

void UCASAnimMasterComponent::OnCombinedAnimEnded(const FGameplayTag& animTag)
{
}

ERelativeDirection UCASAnimMasterComponent::GetCharacterRelativedDirection(const ACharacter* otherChar) const
{
    if (!otherChar || !characterOwner) {
        return ERelativeDirection::EAny;
    }

    FVector masterRelative = characterOwner->GetActorLocation()
        - otherChar->GetActorLocation();

    FVector relativeNormalized = masterRelative.GetUnsafeNormal();

    float forwardDot = FVector::DotProduct(otherChar->GetActorForwardVector(), relativeNormalized);
    float rightDot = FVector::DotProduct(otherChar->GetActorRightVector(), relativeNormalized);
    if (forwardDot >= 0.707f || rightDot >= 0.707f || rightDot <= -0.707f)
        return ERelativeDirection::EFrontal;
    else
        return ERelativeDirection::EOpposite;
}

void UCASAnimMasterComponent::HandleMontageFinished(UAnimMontage* inMontage, bool bInterruptted)
{
    if (currentAnim.MasterAnimConfig.MasterAnimMontage == inMontage) {
        ServerCombinedAnimationEnded.Broadcast(currentAnim.AnimTag);
        DispatchAnimEnded(currentAnim.AnimTag);
  
        UAnimInstance* animinst = (characterOwner->GetMesh()->GetAnimInstance());

        if (animinst) {
            animinst->OnMontageBlendingOut.RemoveDynamic(this, &UCASAnimMasterComponent::HandleMontageFinished);
        }
        bIsPlayingCombAnim = false;
    }
}

void UCASAnimMasterComponent::HandleSnapPointReached(bool bSuccess)
{
    UCASSnapperComponent* snapComponent = characterOwner->FindComponentByClass<UCASSnapperComponent>();
    if (snapComponent) {
        snapComponent->OnSnapPointReached.RemoveDynamic(this, &UCASAnimMasterComponent::HandleSnapPointReached);
    }
}

void UCASAnimMasterComponent::StartAnim()
{
    UAnimInstance* animinst = (characterOwner->GetMesh()->GetAnimInstance());
    if (animinst && currentAnim.AnimSlave && currentAnim.MasterAnimConfig.MasterAnimMontage) {
        animinst->OnMontageBlendingOut.AddDynamic(this, &UCASAnimMasterComponent::HandleMontageFinished);  

        FRotator rotation = characterOwner->GetActorRotation();
        rotation.Yaw = UKismetMathLibrary::FindLookAtRotation(characterOwner->GetActorLocation(), currentAnim.AnimSlave->GetActorLocation()).Yaw;
        characterOwner->SetActorRotation(rotation);
        MulticastPlayAnimMontage(currentAnim.MasterAnimConfig.MasterAnimMontage, characterOwner);
        StartAnimOnSlave();
        ServerCombinedAnimationStarted.Broadcast(currentAnim.AnimTag);
        DispatchAnimStarted(currentAnim.AnimTag);
    }
}

void UCASAnimMasterComponent::StartAnimOnSlave()
{
    if (currentAnim.AnimSlave) {
        const UCASAnimSlaveComponent* slaveComp = currentAnim.AnimSlave->FindComponentByClass<UCASAnimSlaveComponent>();

        FCombinedAnimsSlave outAnim;
        if (slaveComp->TryGetSlaveAnim(currentAnim.AnimTag, outAnim) && outAnim.MasterAnimMontage) {
            FRotator rotation = currentAnim.AnimSlave->GetActorRotation();

            switch (currentAnim.MasterAnimConfig.SlaveForcedDirection) {
            case ERelativeDirection::EFrontal:
                rotation = UKismetMathLibrary::FindLookAtRotation(currentAnim.AnimSlave->GetActorLocation(), characterOwner->GetActorLocation());
                break;
            case ERelativeDirection::EOpposite:
                rotation = UKismetMathLibrary::FindLookAtRotation(currentAnim.AnimSlave->GetActorLocation(), characterOwner->GetActorLocation()).GetInverse();
                break;
            case ERelativeDirection::EAny:
            default:
                break;
            }

            currentAnim.AnimSlave->SetActorRotation(rotation);
            UAnimInstance* animinst = (currentAnim.AnimSlave->GetMesh()->GetAnimInstance());
            if (animinst) {
                MulticastPlayAnimMontage(outAnim.MasterAnimMontage, currentAnim.AnimSlave);
            }
        } else {
            bIsPlayingCombAnim = false;
        }
    }
}

void UCASAnimMasterComponent::DispatchAnimStarted_Implementation(const FGameplayTag& animTag)
{
	OnCombinedAnimationStarted.Broadcast(animTag);
	OnCombinedAnimStarted(animTag);
}

bool UCASAnimMasterComponent::DispatchAnimStarted_Validate(const FGameplayTag& animTag)
{
	return true;
}

void UCASAnimMasterComponent::DispatchAnimEnded_Implementation(const FGameplayTag& animTag)
{
	OnCombinedAnimationEnded.Broadcast(animTag);
	OnCombinedAnimEnded(animTag);
}

bool UCASAnimMasterComponent::DispatchAnimEnded_Validate(const FGameplayTag& animTag)
{
	return true;
}
