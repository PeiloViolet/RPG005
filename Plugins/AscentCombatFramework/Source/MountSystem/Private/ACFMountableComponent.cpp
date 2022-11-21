// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "ACFMountableComponent.h"
#include "ACFDismountPointComponent.h"
#include "GameFramework/Pawn.h"
#include <GameFramework/Character.h>
#include "Components/ACFActionsManagerComponent.h"
#include "ACFRiderComponent.h"
#include "Net/UnrealNetwork.h"
#include "Interfaces/ACFEntityInterface.h"



FName UACFMountableComponent::GetDismountPointTag(const FVector& location) const
{
    const UACFDismountPointComponent* executionPoint = GetNearestDismountPoint(location);
    if (executionPoint) {
        return executionPoint->GetPointTag();
    }

    return NAME_None;
}

class UACFDismountPointComponent* UACFMountableComponent::GetNearestDismountPoint(const FVector& location) const
{
    TArray<UACFDismountPointComponent*> executionPoints;
    GetOwner()->GetComponents<UACFDismountPointComponent>(executionPoints);

    UACFDismountPointComponent* toBeReturned = nullptr;
    float minDistance = BIG_NUMBER;

    for (const auto execPoint : executionPoints) {
        const FVector componentLoc = execPoint->GetComponentLocation();
        FVector distanceV = location - componentLoc;
        float distance;
        distanceV.ToDirectionAndLength(distanceV, distance);
        if (distance < minDistance) {
            toBeReturned = execPoint;
            minDistance = distance;
        }
    }

    return toBeReturned;
}

UACFMountableComponent::UACFMountableComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
    // ...
}

// Called when the game starts
void UACFMountableComponent::BeginPlay()
{
    Super::BeginPlay();
	pawnOwner = Cast<APawn>(GetOwner());

    if (GetPawnOwner()) {
        UACFActionsManagerComponent* actionsComp = GetPawnOwner()->FindComponentByClass<UACFActionsManagerComponent>();
        if (actionsComp) {
            actionsComp->OnActionTriggered.AddDynamic(this, &UACFMountableComponent::HandleActionTriggered);
        }
    } else {
        UE_LOG(LogTemp, Error, TEXT("Mount must be pawn! - UACFMountComponent::BeginPlay"));
    }
}

void UACFMountableComponent::StartMount_Implementation(ACharacter* inRider)
{
    if (!bIsMounted && inRider) {
        UACFRiderComponent* riderComp = inRider->FindComponentByClass<UACFRiderComponent>();
        if (riderComp) {
            riderComp->StartMount(this);
        }
    }
}

bool UACFMountableComponent::StartMount_Validate(class ACharacter* inRider)
{
    return true;
}

void UACFMountableComponent::StopMount_Implementation(const FName& dismountPoint /*= NAME_None*/)
{
    if (rider) {
        UACFRiderComponent* riderComp = rider->FindComponentByClass<UACFRiderComponent>();
        if (riderComp) {
            riderComp->StartDismount(dismountPoint);
        }
    }
}

bool UACFMountableComponent::StopMount_Validate(const FName& dismountPoint /*= NAME_None*/)
{
    return true;
}

FTransform UACFMountableComponent::GetMountPointTransform() const
{
    if (GetPawnOwner() && GetMountMesh()) {
        return GetMountMesh()->GetSocketTransform(MountPointSocket);
    }
    UE_LOG(LogTemp, Error, TEXT("Invalid Mount Point ! - UACFMountComponent::GetMountPointTransform"))
    return FTransform();
}

class UACFDismountPointComponent* UACFMountableComponent::GetDismountPoint(const FName& dismountPoint /*= NAME_None*/) const
{
    TArray<UACFDismountPointComponent*> executionPoints;
    const FName actualDismount = dismountPoint == NAME_None ? DefaultDismountPoint : dismountPoint;
    GetOwner()->GetComponents<UACFDismountPointComponent>(executionPoints);
    for (const auto execPoint : executionPoints) {
        if (execPoint->GetPointTag() == actualDismount) {
            return execPoint;
        }
    }
    return nullptr;
}

USkeletalMeshComponent* UACFMountableComponent::GetMountMesh() const
{
    if (mountMeshOverride) {
        return mountMeshOverride;
    }

    return pawnOwner->FindComponentByClass<USkeletalMeshComponent>();
}

ETeam UACFMountableComponent::GetOwnerTeam() const
{
	const bool bImplements = GetPawnOwner()->GetClass()->ImplementsInterface(UACFEntityInterface::StaticClass());
	if (bImplements)
	{
		return IACFEntityInterface::Execute_GetEntityCombatTeam(GetPawnOwner());
	}
    return ETeam::ENeutral;
}

void UACFMountableComponent::HandleActionTriggered(FGameplayTag ActionState, EActionPriority Priority)
{
    if (rider && ActionsToBeRetriggeredToRider.Contains(ActionState)) {
        UACFActionsManagerComponent* actionsComp = rider->FindComponentByClass<UACFActionsManagerComponent>();
        if (actionsComp) {
            actionsComp->TriggerAction(ActionState, Priority);
        }
    }
}

void UACFMountableComponent::OnRep_IsMounted()
{
    OnMountedStateChanged.Broadcast(bIsMounted);
}

void UACFMountableComponent::SetMounted(bool inMounted)
{
    bIsMounted = inMounted;

    if (!bIsMounted) {
        rider = nullptr;
    }
    OnMountedStateChanged.Broadcast(inMounted);
}

void UACFMountableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UACFMountableComponent, bIsMounted);
    DOREPLIFETIME(UACFMountableComponent, rider);
}
