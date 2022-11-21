#include "RPGNotifyUpdateRotateInfo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BlueprintGameplayTagLibrary.h"
#include "RPG005/ALSExtras/RPGCharacter.h"


void URPGNotifyUpdateRotateInfo::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                                   float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(Mesh, Animation, TotalDuration, EventReference);
	if(Mesh && Mesh->GetOwner())
	{
		Character = Cast<ARPGCharacter>(Mesh->GetOwner());
		if (Character)
		{
			Character->RPGActionTransform.CurrentTime = 0.f;
			Character->RPGActionTransform.Alpha = 0.f;
			Character->RPGActionTransform.TimeSpent = TotalDuration;
			Character->RPGActionTransform.CurrentAnimActionTag = Character->GetActionsComponent()->GetCurrentActionTag();
			Character->RPGActionTransform.StartRotation = Character->GetActorRotation();
			if(Character->RPGTargetInfo.HasTarget)
			{
				Character->RPGActionTransform.InputRotation = Character->RPGTargetInfo.LookAtRotation;
				Character->RPGActionTransform.bIsRefreshRPGActionRotation = true;
			}
			else
			{
				if(Character->LocomotionState.bHasInput)
				{
					const FRotator InputRotation = Character->GetActorRotation();
					Character->RPGActionTransform.InputRotation = {InputRotation.Pitch, Character->LocomotionState.InputYawAngle, InputRotation.Roll};
					Character->RPGActionTransform.bIsRefreshRPGActionRotation = true;
				}
			}
		}
	}
}

void URPGNotifyUpdateRotateInfo::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(Mesh, Animation, EventReference);
	if(Character)
	{
		Character->RPGActionTransform.bIsRefreshRPGActionRotation = false;
	}
}