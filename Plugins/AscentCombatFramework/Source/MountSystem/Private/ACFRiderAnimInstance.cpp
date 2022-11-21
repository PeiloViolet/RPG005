// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ACFRiderAnimInstance.h"
#include <Actors/ACFCharacter.h>
#include "ACFRiderComponent.h"
#include <Animation/ACFAnimInstance.h>

void UACFRiderAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (ACFCharacter && !ACFCharacter->OnCombatTypeChanged.IsAlreadyBound(this, &UACFRiderAnimInstance::HandleCombatTypeChanged)) {
		ACFCharacter->OnCombatTypeChanged.AddDynamic(this, &UACFRiderAnimInstance::HandleCombatTypeChanged);
	}
}

void UACFRiderAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!RiderComp && ACFCharacter) {
		RiderComp = ACFCharacter->FindComponentByClass<UACFRiderComponent>();
		if (RiderComp) {
			RiderComp->OnRidingStateChanged.AddDynamic(this, &UACFRiderAnimInstance::HandleRidingChanged);
		}
	}
	if (bIsRiding && RiderComp) {
		AACFCharacter* mount = Cast< AACFCharacter>(RiderComp->GetMount());
		if (mount) {
			UACFAnimInstance* animInstance = mount->GetACFAnimInstance();
			if (animInstance) {
				MountSpeed = animInstance->GetNormalizedSpeed();
				MountTurn = animInstance->GetTurnRate();
			}
		}
	}
}

void UACFRiderAnimInstance::HandleCombatTypeChanged(ECombatType inCombatType)
{
	CombatType = inCombatType;
}

void UACFRiderAnimInstance::HandleRidingChanged(bool inIsRiding)
{
	bIsRiding = inIsRiding;
}
