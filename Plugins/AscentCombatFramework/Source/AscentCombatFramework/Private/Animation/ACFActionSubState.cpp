// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Animation/ACFActionSubState.h"
#include "Actors/ACFCharacter.h"
#include "Components/ACFActionsManagerComponent.h"
#include <Components/SkeletalMeshComponent.h>
#include <Kismet/KismetSystemLibrary.h>




void UACFActionSubState::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{

	if (MeshComp && MeshComp->GetOwner())
	{
		AACFCharacter* _character = Cast< AACFCharacter>(MeshComp->GetOwner());

		if (_character)
		{
			UACFActionsManagerComponent* _amc = _character->GetActionsComponent();
			if (_amc)
			{
				_amc->StartSubState();
			}
		}
	}
}

void UACFActionSubState::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{

	if (MeshComp && MeshComp->GetOwner())
	{
		AACFCharacter* _character = Cast< AACFCharacter>(MeshComp->GetOwner());

		if (_character)
		{
			UACFActionsManagerComponent* _amc = _character->GetActionsComponent();
			if (_amc)
			{
				_amc->EndSubState();
			}
		}
	}
}
