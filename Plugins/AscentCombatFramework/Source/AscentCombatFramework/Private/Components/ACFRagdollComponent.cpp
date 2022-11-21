// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "Components/ACFRagdollComponent.h"
#include <Components/SkeletalMeshComponent.h>
#include <Components/CapsuleComponent.h>
#include "Game/ACFDamageType.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFDamageHandlerComponent.h"
#include <Animation/AnimMontage.h>
#include <Engine/World.h>
#include <TimerManager.h>

#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Animation/ACFAnimInstance.h"
#include "Components/ACFRagdollMasterComponent.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/GameModeBase.h>

// Sets default values for this component's properties
UACFRagdollComponent::UACFRagdollComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UACFRagdollComponent::BeginPlay()
{
	Super::BeginPlay();

	characterOwner = Cast<AACFCharacter>(GetOwner());

	if (!characterOwner) {
		UE_LOG(LogTemp, Log, TEXT("Ragdoll component must be placed on a ACFCharacter! - UACFRagdollComponent"));
		return;
	}
	UACFDamageHandlerComponent* damageComp = characterOwner->GetDamageHandlerComponent();

	damageComp->OnDamageReceived.AddDynamic(this, &UACFRagdollComponent::HandleDamageReceived);
}




void UACFRagdollComponent::GoRagdoll(const FRagdollImpulse& impulse) {

	if (!GetMesh() || !GetCapsuleComponent()) {
		return;
	}
 	const FVector beforeLoc = GetMesh()->GetRelativeLocation();
 	const FQuat beforeRot = GetMesh()->GetRelativeRotation().Quaternion();

	if (!bIsKinematic) {
		beforeRagdollTransform.SetLocation(beforeLoc);
		beforeRagdollTransform.SetRotation(beforeRot);
	}

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBone, true);

	const FVector impulseVec = impulse.ImpulseDirection.GetSafeNormal() * impulse.ImpulseIntensity;
	
	characterOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetMesh()->AddImpulse(impulseVec, impulse.BoneName, true);
	SetIsRagdoll(true);
}

void UACFRagdollComponent::GoRagdollForDuration(const FRagdollImpulse& impulse, float ragdollDuration)
{
	GoRagdoll(impulse);
	UWorld* world = GetWorld();
	if (world) {
		FTimerHandle ragdollTimer;
		world->GetTimerManager().SetTimer(
			ragdollTimer, this, &UACFRagdollComponent::RecoverFromRagdoll,
			ragdollDuration, false);	
	}
	
}

void UACFRagdollComponent::GoRagdollFromDamage(const FACFDamageEvent& damageEvent, float RagdollDuration /*= -1.f*/)
{
	FRagdollImpulse impulseRag;

	if (!damageEvent.DamageClass) {
		return;
	}

	const UDamageType* damageObj = damageEvent.DamageClass.GetDefaultObject();
	impulseRag.ImpulseIntensity = damageObj->DamageImpulse;
	impulseRag.BoneName = damageEvent.hitResult.BoneName;
	impulseRag.ImpulseDirection = -damageEvent.hitDirection;

	GoRagdollForDuration(impulseRag, RagdollDuration);
}

void UACFRagdollComponent::RecoverFromRagdoll() {

	if (!bIsRagdoll) {
		return;
	}

	if (!GetMesh() || !GetCapsuleComponent()) {
		return;
	}

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBone, false);
	UpdateOwnerLocation();
	if (!bIsKinematic) {
		GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	 	GetMesh()->SetRelativeLocation(beforeRagdollTransform.GetLocation());
	 	GetMesh()->SetRelativeRotation(beforeRagdollTransform.GetRotation());
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
// 	UACFAnimInstance* animInstance = characterOwner->GetACFAnimInstance();
// 	if (animInstance) {
// 		animInstance->StopBlendSnapshotPose();
// 	}
/*	bGettingUp = true;*/
	characterOwner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	UAnimMontage* montage = GetGetUpMontage();
	if (montage) {
		characterOwner->PlayNetworkedMontage(montage);
	}

	SetIsRagdoll(false);
}

void UACFRagdollComponent::HandleDamageReceived(const FACFDamageEvent& damageEvent)
{
	if (!bTestRagdollOnHit) {
		return;
	}

	if (!damageEvent.DamageClass) {
		return;
	}

	const UDamageType* damageObj = damageEvent.DamageClass.GetDefaultObject();

	if (damageObj->DamageImpulse >= ImpulseResistance) {
		GoRagdollFromDamage(damageEvent, DefaultRagdollDuration);
	}

}



void UACFRagdollComponent::UpdateOwnerLocation()
{
	if (characterOwner) {
		FVector locDest = GetMesh()->GetSocketLocation(PelvisBone);
		locDest.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		characterOwner->SetActorLocation(locDest);
	}

}

bool UACFRagdollComponent::IsFaceUp()
{
	return (GetMesh()->GetSocketRotation(PelvisBone).Yaw < 0.f);
}


void UACFRagdollComponent::SetIsRagdoll(bool inIsRagdoll)
{
	bIsRagdoll = inIsRagdoll;

	AGameModeBase* gameMode = UGameplayStatics::GetGameMode(this);
	if (gameMode) {
		UACFRagdollMasterComponent* ragdollMaster = gameMode->FindComponentByClass<UACFRagdollMasterComponent>();
		if (ragdollMaster) {	
			if (bIsRagdoll) {
				ragdollMaster->AddComponent(this);
			}
			else {
				ragdollMaster->RemoveComponent(this);
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Add Ragdoll Master to your Game Mode!"));
		}
	}
	OnRagdollStateChanged.Broadcast(bIsRagdoll);
	
}

UAnimMontage* UACFRagdollComponent::GetGetUpMontage()
{
	if (IsFaceUp()) {
		return GetUpFront;
	}
	return GetUpBack;
}
