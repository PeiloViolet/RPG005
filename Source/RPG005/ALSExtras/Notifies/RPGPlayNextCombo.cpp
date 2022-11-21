#include "RPGPlayNextCombo.h"


void URPGPlayNextCombo::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
									   float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(Mesh, Animation, TotalDuration, EventReference);
	if (Mesh && Mesh->GetOwner())
	{
		Character = Cast<ARPGCharacter>(Mesh->GetOwner());
		AlsAnimationInstance = Cast<UAlsAnimationInstance>(Mesh->GetAnimInstance());
		if (Character)
		{
			ActionsManagerComponent = Character->GetActionsComponent();
		}
	}
}

void URPGPlayNextCombo::NotifyTick(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(Mesh, Animation, FrameDeltaTime, EventReference);
	
	if (Character && AlsAnimationInstance && Character->CanSkipCharge)
	{
		ActionsManagerComponent->StoreAction(AlsLocomotionActionTags::HeavyAttack);
		ActionsManagerComponent->FreeAction();
		
		AlsAnimationInstance->Montage_JumpToSectionsEnd(AlsAnimationInstance->Montage_GetCurrentSection());

		Character->CanSkipCharge = false;
	}
}

void URPGPlayNextCombo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (Character && AlsAnimationInstance)
	{
		//ActionsManagerComponent->StoreAction(AlsLocomotionActionTags::HeavyAttack);
		//ActionsManagerComponent->FreeAction();
		Character->CanSkipCharge = false;
	}
}
