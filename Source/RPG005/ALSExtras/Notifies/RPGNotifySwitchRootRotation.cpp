#include "RPGNotifySwitchRootRotation.h"

#include "RPG005/ALS/AlsCharacterMovementComponent.h"

void URPGNotifySwitchRootRotation::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(Mesh, Animation, TotalDuration, EventReference);
	if (Mesh && Mesh->GetOwner())
	{
		Character = Cast<ARPGCharacter>(Mesh->GetOwner());
		if (Character)
		{
			AlsCharacterMovementComponent = Cast<UAlsCharacterMovementComponent>(Character->GetMovementComponent());
			if (AlsCharacterMovementComponent)
			{
				AlsCharacterMovementComponent->bAllowPhysicsRotationDuringAnimRootMotion = false;
			}
		}
	}
}
void URPGNotifySwitchRootRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (AlsCharacterMovementComponent)
	{
		AlsCharacterMovementComponent->bAllowPhysicsRotationDuringAnimRootMotion = true;
	}
}
