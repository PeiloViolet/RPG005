#include "RPGNotifyExitAction.h"
#include "Actors/ACFCharacter.h"
#include <Components/SkeletalMeshComponent.h>
#include "RPG005/ALSExtras/RPGCharacter.h"
#include "RPG005/ALS/AlsAnimationInstance.h"
#include "Animation/AnimInstance.h"

void URPGNotifyExitAction::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
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
			if (ActionsManagerComponent)
			{
				
				//ActionsManagerComponent->FreeAction();//如果有，则自动释放存储动作
				
			}
		}
	}
}

void URPGNotifyExitAction::NotifyTick(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(Mesh, Animation, FrameDeltaTime, EventReference);
	if (Character)
	{
		if (Character->RPGLocomotionDirection != ERPGLocomotionDirection::None)
		{
			if (AlsAnimationInstance)
			{
				if (ActionsManagerComponent)
				{
					ActionsManagerComponent->ExitAction();
				}
				AlsAnimationInstance->Montage_Stop(BlendOutTime,AlsAnimationInstance->GetCurrentActiveMontage());
			}
		}
	}
}

void URPGNotifyExitAction::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
}
