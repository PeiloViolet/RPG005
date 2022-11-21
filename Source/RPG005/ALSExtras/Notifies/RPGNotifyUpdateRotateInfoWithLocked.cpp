#include "RPGNotifyUpdateRotateInfoWithLocked.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BlueprintGameplayTagLibrary.h"
#include "RPG005/ALSExtras/RPGCharacter.h"

void URPGNotifyUpdateRotateInfoWithLocked::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
												   float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(Mesh, Animation, TotalDuration, EventReference);
	if(Mesh && Mesh->GetOwner())
	{
		Character = Cast<ARPGCharacter>(Mesh->GetOwner());
		if (Character)
		{
			if(Character->RPGTargetInfo.HasTarget)
			{
				Character->RPGActionTransform.CurrentAnimActionTag = Character->GetActionsComponent()->GetCurrentActionTag();
				Character->RPGActionTransform.bIsRefreshRPGActionRotationWithLocked = true;
			}
		}
	}
}

void URPGNotifyUpdateRotateInfoWithLocked::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
}

void URPGNotifyUpdateRotateInfoWithLocked::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                     const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(Mesh, Animation, EventReference);
	if(Character)
	{
		Character->RPGActionTransform.bIsRefreshRPGActionRotationWithLocked = false;
	}
}
