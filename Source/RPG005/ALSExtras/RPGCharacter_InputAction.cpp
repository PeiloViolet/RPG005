#include "RPGCharacter.h"
#include "BlueprintGameplayTagLibrary.h"
#include "CCMCameraFunctionLibrary.h"
#include "CCMPlayerCameraManager.h"
#include "RPG005/ALSCamera/AlsCameraComponent.h"
#include "Game/ACFFunctionLibrary.h"
#include "InputActionValue.h"
#include "RPG_Controller.h"
#include "Components/ACFInteractionComponent.h"
#include "Components/ACFLocomotionComponent.h"
#include "RPG005/ALS/AlsAnimationInstance.h"
#include "Utility/Interface/AttackInterface.h"

void ARPGCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	auto* EnhancedInput{Cast<UEnhancedInputComponent>(Input)};
	if (IsValid(EnhancedInput))
	{
		EnhancedInput->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::InputLookMouse);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::InputLook);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ThisClass::InputSprint);
		EnhancedInput->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ThisClass::InputDodge);
		EnhancedInput->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ThisClass::InputWalk);
		EnhancedInput->BindAction(AlsCrouchAction, ETriggerEvent::Triggered, this, &ThisClass::InputCrouch);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::InputJump);
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::InputAim);
		EnhancedInput->BindAction(RagdollAction, ETriggerEvent::Triggered, this, &ThisClass::InputRagdoll);
		EnhancedInput->BindAction(RollAction, ETriggerEvent::Triggered, this, &ThisClass::InputRoll);
		EnhancedInput->BindAction(RotationModeAction, ETriggerEvent::Triggered, this, &ThisClass::InputRotationMode);
		EnhancedInput->BindAction(ViewModeAction, ETriggerEvent::Triggered, this, &ThisClass::InputViewMode);
		EnhancedInput->BindAction(SwitchShoulderAction, ETriggerEvent::Triggered, this, &ThisClass::InputSwitchShoulder);
		EnhancedInput->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ThisClass::InputInteract);
		EnhancedInput->BindAction(TargetAction, ETriggerEvent::Triggered, this, &ThisClass::InputTarget);
		EnhancedInput->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ThisClass::InputEquip);
		EnhancedInput->BindAction(LightAttackAction, ETriggerEvent::Started, this, &ThisClass::InputLightAttack);
		EnhancedInput->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ThisClass::InputHeavyAttack_Started);
		EnhancedInput->BindAction(HeavyAttackAction, ETriggerEvent::Ongoing, this, &ThisClass::InputHeavyAttack_Ongoing);
		EnhancedInput->BindAction(HeavyAttackAction, ETriggerEvent::Triggered, this, &ThisClass::InputHeavyAttack_Triggered);
	}
}

void ARPGCharacter::InputLookMouse(const FInputActionValue& ActionValue)
{
	const auto Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * LookUpMouseSensitivity);
	AddControllerYawInput(Value.X * LookRightMouseSensitivity);
}

void ARPGCharacter::InputLook(const FInputActionValue& ActionValue)
{
	const auto Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * LookUpRate * GetWorld()->GetDeltaSeconds());
	AddControllerYawInput(Value.X * LookRightRate * GetWorld()->GetDeltaSeconds());
}

void ARPGCharacter::InputMove(const FInputActionValue& ActionValue)
{
	const auto Value{UAlsMath::ClampMagnitude012D(ActionValue.Get<FVector2D>())};
	
	LocomotionComp->MoveForward(Value.Y);
	LocomotionComp->MoveRight(Value.X);
	
}

void ARPGCharacter::InputSprint_Implementation(const FInputActionValue& ActionValue)
{
	if(ActionValue.Get<bool>())
	{
		StartSprint();
	}
	else
	{
		StopSprint();
	}
}

void ARPGCharacter::InputWalk()
{
	if (GetDesiredGait() == AlsGaitTags::Walking)
	{
		SetDesiredGait(AlsGaitTags::Running);
	}
	else if (GetDesiredGait() == AlsGaitTags::Running)
	{
		SetDesiredGait(AlsGaitTags::Walking);
	}
}

void ARPGCharacter::InputCrouch()
{
	if (GetDesiredStance() == AlsStanceTags::Standing)
	{
		SetDesiredStance(AlsStanceTags::Crouching);
	}
	else if (GetDesiredStance() == AlsStanceTags::Crouching)
	{
		SetDesiredStance(AlsStanceTags::Standing);
	}
}

void ARPGCharacter::InputJump(const FInputActionValue& ActionValue)
{
	if (ActionValue.Get<bool>())
	{
		if (TryStopRagdolling())
		{
			return;
		}

		if (TryStartMantlingGrounded())
		{
			return;
		}

		if (GetStance() == AlsStanceTags::Crouching)
		{
			SetDesiredStance(AlsStanceTags::Standing);
			return;
		}
		if(AnimationInstance->IsAnyMontagePlaying())
		{
			return;
		}
		Jump();
	}
	else
	{
		StopJumping();
	}
}

void ARPGCharacter::InputAim(const FInputActionValue& ActionValue)
{
	SetDesiredAiming(ActionValue.Get<bool>());
}

void ARPGCharacter::InputRagdoll()
{
	if (!TryStopRagdolling())
	{
		StartRagdolling();
	}
}

void ARPGCharacter::InputRoll()
{
	static constexpr auto PlayRate{1.3f};

	TryStartRolling(PlayRate);
}

void ARPGCharacter::InputDodge()
{
	StartDodging();
}

void ARPGCharacter::InputRotationMode()
{
	SetDesiredRotationMode(GetDesiredRotationMode() == AlsRotationModeTags::VelocityDirection
		                       ? AlsRotationModeTags::LookingDirection
		                       : AlsRotationModeTags::VelocityDirection);
}

void ARPGCharacter::InputViewMode()
{
	SetViewMode(GetViewMode() == AlsViewModeTags::ThirdPerson ? AlsViewModeTags::FirstPerson : AlsViewModeTags::ThirdPerson);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ARPGCharacter::InputSwitchShoulder()
{
	Camera->SetRightShoulder(!Camera->IsRightShoulder());
}

void ARPGCharacter::InputInteract()
{
	if(LocomotionAction == AlsLocomotionActionTags::None)
	{
		InteractComp->Interact();
	}
}

void ARPGCharacter::InputTarget()
{
	if(RPG_Controller)
	{
		RPG_Controller->TargetingComponent->ToggleTargeting();
	}
}

void ARPGCharacter::InputEquip()
{
	if (LocomotionAction == AlsLocomotionActionTags::Ragdolling)
	{
		return;
	}
	TriggerAction(AlsLocomotionActionTags::Equip,EActionPriority::ELow);
}

void ARPGCharacter::InputLightAttack()
{
	if (LocomotionAction == AlsLocomotionActionTags::Ragdolling)
	{
		return;
	}
	
	if(GetCombatType() == ECombatType::EUnarmed)
	{
		TriggerAction(AlsLocomotionActionTags::Equip,EActionPriority::ELow);
		return;
	}
	if(UBlueprintGameplayTagLibrary::MatchesTag(LocomotionAction,AlsLocomotionActionTags::Dodge,false))
	{
		TriggerAction(AlsLocomotionActionTags::RollAttack,EActionPriority::ELow);
		return;
	}
	if((GetGait() == AlsGaitTags::Sprinting && LocomotionComp->GetCurrentLocomotionState() == ELocomotionState::ESprint) || 
	   (UBlueprintGameplayTagLibrary::MatchesTag(ActionsComp->GetCurrentActionTag(),AlsLocomotionActionTags::Step_Back,false)))
	{
		TriggerAction(AlsLocomotionActionTags::SprintAttack,EActionPriority::ELow);
		return;
	}
		TriggerAction(AlsLocomotionActionTags::LightAttack,EActionPriority::ELow);
}

void ARPGCharacter::InputHeavyAttack_Started(const FInputActionValue& ActionValue)
{
	//IsChargeAtack = true;
	CanSkipCharge = false;
	
	if(GetGait() == AlsGaitTags::Sprinting && LocomotionComp->GetCurrentLocomotionState() == ELocomotionState::ESprint)
	{
		TriggerAction(AlsLocomotionActionTags::JumpAttack,EActionPriority::ELow);
	}
	else
	{
	    TriggerAction(AlsLocomotionActionTags::HeavyAttack,EActionPriority::ELow);
	}
}

void ARPGCharacter::InputHeavyAttack_Ongoing(const FInputActionValue& ActionValue)
{
	
}

void ARPGCharacter::InputHeavyAttack_Triggered(const FInputActionValue& ActionValue)
{
	//IsChargeAtack = false;

	CanSkipCharge = true;
	
	
}


