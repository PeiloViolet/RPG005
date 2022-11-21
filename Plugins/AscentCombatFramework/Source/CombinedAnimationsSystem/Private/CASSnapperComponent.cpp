// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "CASSnapperComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include <Math/UnrealMathUtility.h>
#include <Components/SceneComponent.h>
#include <GameFramework/Actor.h>

// Sets default values for this component's properties
UCASSnapperComponent::UCASSnapperComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetComponentTickEnabled(false);
}


// Called when the game starts
void UCASSnapperComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
	// ...
	
}


// Called every frame
void UCASSnapperComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (currentTime >= timeout) {
		TerminateSnap(false);
	}
	if (snapType == ESnapType::EToActor && targetActor) {
		snapPoint = targetActor->GetActorLocation();
	}
	else if (snapType == ESnapType::EToComponent && targetComponent) {
		snapPoint = targetComponent->GetComponentLocation();
	}
	else if (snapType == ESnapType::EToActor && !targetActor){
		TerminateSnap(false);
		return;
	}

	if (!owner) {
		TerminateSnap(false);
		return;
	}

	if (!owner->HasAuthority()) {
		TerminateSnap(false);
		return;
	}

	FHitResult outHit;
	if (FVector::Dist2D(snapPoint, owner->GetActorLocation()) > tolerance) {
		const FVector owenerLoc = owner->GetActorLocation();
		const FVector newLocation = FMath::VInterpTo(owner->GetActorLocation(), snapPoint, DeltaTime, speed);
		owner->SetActorLocation(newLocation, true, &outHit, ETeleportType::TeleportPhysics);
		currentTime += DeltaTime;
		if (bOrientToMov) {
			const FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(owenerLoc, snapPoint);
			FRotator interpLookAt = owner->GetActorRotation();
			interpLookAt.Yaw = FMath::FInterpTo(owner->GetActorRotation().Yaw, lookAt.Yaw, DeltaTime, speed);
			owner->SetActorRotation(interpLookAt);
		}
	}
	else {
		TerminateSnap(true);
	}
}

void UCASSnapperComponent::SnapToPoint(const FVector& snapLocation, float snapSpeed /*= 2.f*/, float distanceTolerance /*= 5.f*/, float timeoutTime /*= 2.f*/, bool bOrientToSnapPoint /*= true*/)
{
	if (bIsSnapping) {
		return;
	}

	snapPoint = snapLocation;
	speed = snapSpeed;
	tolerance = distanceTolerance;
	timeout = timeoutTime;
	currentTime = 0.f;
	owner = GetOwner();
	snapType = ESnapType::EToPoint;
	bOrientToMov = bOrientToSnapPoint;
	if (owner) {
		StartSnap();
	}
	else {
		OnSnapPointReached.Broadcast(false);
	}
}

void UCASSnapperComponent::SnapToActor( AActor* inActor, float snapSpeed /*= 2.f*/, float distanceTolerance /*= 5.f*/, float timeoutTime /*= 2.f*/, bool bOrientToSnapPoint /*= true*/)
{
	if (bIsSnapping) {
		return;
	}

	speed = snapSpeed;
	tolerance = distanceTolerance;
	timeout = timeoutTime;
	currentTime = 0.f;
	owner = GetOwner();
	snapType = ESnapType::EToActor;
	targetActor = inActor;
	bOrientToMov = bOrientToSnapPoint;

	if (owner && targetActor ) {
		StartSnap();
	}
	else {
		OnSnapPointReached.Broadcast(false);
	}
}

void UCASSnapperComponent::SnapToComponent(USceneComponent* inComponent, float snapSpeed /*= 2.f*/, float distanceTolerance /*= 5.f*/, float timeoutTime /*= 2.f*/, bool bOrientToSnapPoint /*= true*/)
{
	if (bIsSnapping) {
		return;
	}

	speed = snapSpeed;
	tolerance = distanceTolerance;
	timeout = timeoutTime;
	currentTime = 0.f;
	owner = GetOwner();
	snapType = ESnapType::EToComponent;
	targetComponent = inComponent;
	bOrientToMov = bOrientToSnapPoint;

	if (owner && targetComponent) {
		StartSnap();
	}
	else {
		OnSnapPointReached.Broadcast(false);
	}
}

void UCASSnapperComponent::TerminateSnap(bool bSuccess)
{
	SetComponentTickEnabled(false);
	currentTime = 0.f;
	OnSnapPointReached.Broadcast(bSuccess);
	bIsSnapping = false;

}

void UCASSnapperComponent::StartSnap()
{
	SetComponentTickEnabled(true);
	bIsSnapping = true;
}

