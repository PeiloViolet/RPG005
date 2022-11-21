#include "RPGAIComboNotify.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

void URPGAIComboNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		AACFCharacter* ACFCharacter = Cast< AACFCharacter>(MeshComp->GetOwner());

		if (ACFCharacter)
		{
			UACFActionsManagerComponent* ActionsManagerComponent = ACFCharacter->GetActionsComponent();
			Controller = Cast<AACFAIController>(UAIBlueprintHelperLibrary::GetAIController(ACFCharacter));
			if (ActionsManagerComponent)
			{
				ActionsManagerComponent->FreeAction();
				if(NextComboActionData != nullptr && Controller)
				{
					CombatBehaviourComponent = Controller->GetCombatBehavior();
					if(CombatBehaviourComponent)
					{
						CombatBehaviourComponent->RPGExecuteAINextCombo(NextComboActionData->ConditionActions);
					}
				}
			}
		}
	}
}
