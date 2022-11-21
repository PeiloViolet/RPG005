// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Animation/ACFNotifyAction.h"
#include <Components/SkeletalMeshComponent.h>
#include "Components/ACFActionsManagerComponent.h"
#include "Actors/ACFCharacter.h"
#include <Kismet/KismetSystemLibrary.h>




void UACFNotifyAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
// 	if (bActivateOnlyOnServer && !UKismetSystemLibrary::IsServer(this))
// 	{
// 		return;
// 	}

	if (MeshComp && MeshComp->GetOwner())
	{
		AACFCharacter* _character = Cast< AACFCharacter>(MeshComp->GetOwner());
	
		if (_character)
		{
			UACFActionsManagerComponent* _amc = _character->GetActionsComponent();
			if (_amc)
			{
				_amc->AnimationsReachedNotablePoint();
			}
		}
	}
}
