#include "NextActionNotify.h"


void UNextActionNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		AACFCharacter* ACFCharacter = Cast< AACFCharacter>(MeshComp->GetOwner());

		if (ACFCharacter)
		{
			UACFActionsManagerComponent* ActionsManagerComponent = ACFCharacter->GetActionsComponent();
			if (ActionsManagerComponent)
			{
				ActionsManagerComponent->FreeAction();
				if(ActionTag.IsValid())
				{
					ActionsManagerComponent->TriggerAction(ActionTag, Priority);
				}
			}
		}
	}
}