// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Animation/ACFNotifyExitAction.h"
#include "Actors/ACFCharacter.h"
#include <Components/SkeletalMeshComponent.h>




void UACFNotifyExitAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		AACFCharacter* _character = Cast< AACFCharacter>(MeshComp->GetOwner());

		if (_character)
		{
			UACFActionsManagerComponent* _amc = _character->GetActionsComponent();
			if (_amc)
			{
				_amc->FreeAction();
			}
		}
	}
}
