#include "RPGNotifySkipCharge.h"
URPGNotifySkipCharge::URPGNotifySkipCharge()
{
	bIsNativeBranchingPoint = true;
}
void URPGNotifySkipCharge::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
									   float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(Mesh, Animation, TotalDuration, EventReference);
	if (Mesh && Mesh->GetOwner())
	{
		Character = Cast<ARPGCharacter>(Mesh->GetOwner());
		AlsAnimationInstance = Cast<UAlsAnimationInstance>(Mesh->GetAnimInstance());
		if (Character && AlsAnimationInstance )
		{
			CurrentMontage = Character->GetCurrentMontage();
			CurrentSectionName = AlsAnimationInstance->Montage_GetCurrentSection(CurrentMontage);
			CurrentSectionIndex = CurrentMontage->GetSectionIndex(CurrentSectionName);
			NextSectionIndex = CurrentSectionIndex + 1;
			NextSectionName = CurrentMontage->GetSectionName(NextSectionIndex);
		}
	}
}

void URPGNotifySkipCharge::NotifyTick(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(Mesh, Animation, FrameDeltaTime, EventReference);
	
	if (Character && AlsAnimationInstance && CurrentMontage && NextSectionName != NAME_None && Character->CanSkipCharge)
	{
		Character->PlayNetworkedMontage(CurrentMontage,1.f, NextSectionName, 1.f);
		Character->CanSkipCharge = false;
	}
}

void URPGNotifySkipCharge::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (Character && AlsAnimationInstance)
	{
		Character->CanSkipCharge = false;
	}
}
