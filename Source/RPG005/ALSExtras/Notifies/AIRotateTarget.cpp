#include "AIRotateTarget.h"

#include <Components/SkeletalMeshComponent.h>

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"


UAIRotateTarget::UAIRotateTarget()
{
	bIsNativeBranchingPoint = true;
}

void UAIRotateTarget::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
                                  float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(Mesh, Animation, TotalDuration, EventReference);
	if (Mesh && Mesh->GetOwner())
	{
		Character = Cast<AACFCharacter>(Mesh->GetOwner());
		
		if (Character)
		{
			AIController = Cast<AACFAIController>(UAIBlueprintHelperLibrary::GetAIController(Character));
			if(AIController && AIController->HasTarget())
			{
				TargetCharacter = Cast<AACFCharacter>(AIController->GetTarget());
			}
		}
	}
}

void UAIRotateTarget::NotifyTick(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(Mesh, Animation, FrameDeltaTime, EventReference);
	if (Character && AIController && TargetCharacter)
	{
		Character->SetActorRotation(UKismetMathLibrary::RInterpTo(Character->GetActorRotation(), UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), TargetCharacter->GetActorLocation()), FrameDeltaTime, InlerpSpeed));
	}
}

void UAIRotateTarget::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}