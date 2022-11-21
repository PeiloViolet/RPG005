#include "RPG_SetLocomotionActionAndExit_Notify.h"
#include "Components/SkeletalMeshComponent.h"
#include <Components/SkeletalMeshComponent.h>
#include "RPG005/ALSExtras/RPGCharacter.h"
#include "RPG005/ALS/AlsAnimationInstance.h"
#include "Animation/AnimInstance.h"
#include "RPG005/ALS/Utility/AlsUtility.h"
#include "RPG005/ALSExtras/RPGCharacter.h"

URPG_SetLocomotionActionAndExit_Notify::URPG_SetLocomotionActionAndExit_Notify()
{
	bIsNativeBranchingPoint = true;
}

FString URPG_SetLocomotionActionAndExit_Notify::GetNotifyName_Implementation() const
{
	return FString::Format(TEXT("RPG Set Locomotion Action: {0}"), {
							   FName::NameToDisplayString(UAlsUtility::GetSimpleTagName(LocomotionAction).ToString(), false)
						   });
}

void URPG_SetLocomotionActionAndExit_Notify::NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
														  const float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(Mesh, Animation, TotalDuration, EventReference);

	auto* Character{Cast<AAlsCharacter>(Mesh->GetOwner())};
	if (IsValid(Character))
	{
		Character->SetLocomotionAction(LocomotionAction);
	}
}

void URPG_SetLocomotionActionAndExit_Notify::NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
														const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(Mesh, Animation, EventReference);

	auto* Character{Cast<ARPGCharacter>(Mesh->GetOwner())};
	
	if (IsValid(Character) && Character->GetLocomotionAction() == LocomotionAction)
	{
		Character->SetLocomotionAction(FGameplayTag::EmptyTag);
	}
	auto* AlsAnimationInstance = Cast<UAlsAnimationInstance>(Mesh->GetAnimInstance());
	if (AlsAnimationInstance && IsValid(Character))
	{
		auto* ActionsManagerComponent = Character->GetActionsComponent();
		if (ActionsManagerComponent)
		{
			ActionsManagerComponent->FreeAction();//如果有，则自动释放存储动作
		}
	}
}
