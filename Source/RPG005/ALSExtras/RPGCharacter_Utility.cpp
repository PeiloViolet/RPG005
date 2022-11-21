#include "RPGCharacter.h"
#include "ATSTargetableInterface.h"
#include "Game/ACFFunctionLibrary.h"
#include "Components/ACFLocomotionComponent.h"
#include "RPG005/ALS/AlsAnimationInstance.h"

void ARPGCharacter::StartSprint()
{
	SetDesiredGait(AlsGaitTags::Sprinting);
	LocomotionComp->AccelerateToNextState();
}


void ARPGCharacter::StopSprint()
{
	SetDesiredGait(AlsGaitTags::Running);
	if (ELocomotionState::ESprint == LocomotionComp->GetCurrentLocomotionState())
	{
		LocomotionComp->SetLocomotionState(ELocomotionState::EJog);
	}
}

void ARPGCharacter::StartDodging()
{
	const FGameplayTag DodgeActionTag{SelectDodgeActionTag()};

	TriggerAction(DodgeActionTag,EActionPriority::ELow);
}

FGameplayTag ARPGCharacter::SelectDodgeActionTag_Implementation()
{
	if(RPGLocomotionDirection == ERPGLocomotionDirection::None)
	{
		return AlsLocomotionActionTags::Step_Back;
	}
	if (RotationMode == AlsRotationModeTags::VelocityDirection)
	{
		return AlsLocomotionActionTags::Dodge_F;
	}
	switch (RPGLocomotionDirection)
	{
	case ERPGLocomotionDirection::None:return AlsLocomotionActionTags::Step_Back;
	case ERPGLocomotionDirection::Forward:return AlsLocomotionActionTags::Dodge_F;
	case ERPGLocomotionDirection::Backward:return AlsLocomotionActionTags::Dodge_B;
	case ERPGLocomotionDirection::Left:return AlsLocomotionActionTags::Dodge_L;
	case ERPGLocomotionDirection::Right:return AlsLocomotionActionTags::Dodge_R;
	case ERPGLocomotionDirection::Forward_Left:return AlsLocomotionActionTags::Dodge_FL;
	case ERPGLocomotionDirection::Forward_Right:return AlsLocomotionActionTags::Dodge_FR;
	case ERPGLocomotionDirection::Backward_Left:return AlsLocomotionActionTags::Dodge_BL;
	case ERPGLocomotionDirection::Backward_Right:return AlsLocomotionActionTags::Dodge_BR;
	}
	return AlsLocomotionActionTags::Step_Back;
}

void ARPGCharacter::OnTargetAcquired_Implementation(UATSTargetPointComponent* point)
{
	IATSTargetableInterface::OnTargetAcquired_Implementation(point);
	GetsTargeted(point);
	
}

void ARPGCharacter::OnTargetLosed_Implementation()
{
	IATSTargetableInterface::OnTargetLosed_Implementation();
	LoseTargeted();
}

void ARPGCharacter::GetsTargeted_Implementation(UATSTargetPointComponent* point)
{
	
}

void ARPGCharacter::LoseTargeted_Implementation()
{
	
}

void ARPGCharacter::OnTargetChangedSetRotationMode(bool bIsTargeting)
{
	if(bIsTargeting)
	{
		RPGTargetInfo.HasTarget = true;
		SetDesiredRotationMode(AlsRotationModeTags::LookingDirection);
	}
	else
	{
		RPGTargetInfo.HasTarget = false;
		SetDesiredRotationMode(AlsRotationModeTags::VelocityDirection);
	}
}
