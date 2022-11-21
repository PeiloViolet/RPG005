#include "AlsGameplayTags.h"

namespace AlsViewModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(FirstPerson, TEXT("Als.ViewMode.FirstPerson"))
	UE_DEFINE_GAMEPLAY_TAG(ThirdPerson, TEXT("Als.ViewMode.ThirdPerson"))
}

namespace AlsLocomotionModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Grounded, TEXT("Als.LocomotionMode.Grounded"))
	UE_DEFINE_GAMEPLAY_TAG(InAir, TEXT("Als.LocomotionMode.InAir"))
}

namespace AlsRotationModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(VelocityDirection, TEXT("Als.RotationMode.VelocityDirection"))
	UE_DEFINE_GAMEPLAY_TAG(LookingDirection, TEXT("Als.RotationMode.LookingDirection"))
	UE_DEFINE_GAMEPLAY_TAG(Aiming, TEXT("Als.RotationMode.Aiming"))
}

namespace AlsStanceTags
{
	UE_DEFINE_GAMEPLAY_TAG(Standing, TEXT("Als.Stance.Standing"))
	UE_DEFINE_GAMEPLAY_TAG(Crouching, TEXT("Als.Stance.Crouching"))
}

namespace AlsGaitTags
{
	UE_DEFINE_GAMEPLAY_TAG(Walking, TEXT("Als.Gait.Walking"))
	UE_DEFINE_GAMEPLAY_TAG(Running, TEXT("Als.Gait.Running"))
	UE_DEFINE_GAMEPLAY_TAG(Sprinting, TEXT("Als.Gait.Sprinting"))
}

namespace AlsOverlayModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(Default, TEXT("Als.OverlayMode.Default"))
	UE_DEFINE_GAMEPLAY_TAG(StraightSword, TEXT("Als.OverlayMode.StraightSword"))
	UE_DEFINE_GAMEPLAY_TAG(Masculine, TEXT("Als.OverlayMode.Masculine"))
	UE_DEFINE_GAMEPLAY_TAG(Feminine, TEXT("Als.OverlayMode.Feminine"))
	UE_DEFINE_GAMEPLAY_TAG(Injured, TEXT("Als.OverlayMode.Injured"))
	UE_DEFINE_GAMEPLAY_TAG(HandsTied, TEXT("Als.OverlayMode.HandsTied"))
	UE_DEFINE_GAMEPLAY_TAG(M4, TEXT("Als.OverlayMode.M4"))
	UE_DEFINE_GAMEPLAY_TAG(PistolOneHanded, TEXT("Als.OverlayMode.PistolOneHanded"))
	UE_DEFINE_GAMEPLAY_TAG(PistolTwoHanded, TEXT("Als.OverlayMode.PistolTwoHanded"))
	UE_DEFINE_GAMEPLAY_TAG(Bow, TEXT("Als.OverlayMode.Bow"))
	UE_DEFINE_GAMEPLAY_TAG(Torch, TEXT("Als.OverlayMode.Torch"))
	UE_DEFINE_GAMEPLAY_TAG(Binoculars, TEXT("Als.OverlayMode.Binoculars"))
	UE_DEFINE_GAMEPLAY_TAG(Box, TEXT("Als.OverlayMode.Box"))
	UE_DEFINE_GAMEPLAY_TAG(Barrel, TEXT("Als.OverlayMode.Barrel"))
}

namespace AlsBaseLayerModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(AlsBaseLayer, TEXT("Als.LocomotionLayer.AlsBaseLayer"))
	UE_DEFINE_GAMEPLAY_TAG(RPGFemaleLayer, TEXT("Als.LocomotionLayer.RPGFemaleLayer"))
}

namespace AlsLocomotionActionTags
{
	UE_DEFINE_GAMEPLAY_TAG(None, TEXT("Als.LocomotionAction.None"))
	UE_DEFINE_GAMEPLAY_TAG(Mantling, TEXT("Als.LocomotionAction.Mantling"))
	UE_DEFINE_GAMEPLAY_TAG(Ragdolling, TEXT("Als.LocomotionAction.Ragdolling"))
	UE_DEFINE_GAMEPLAY_TAG(GettingUp, TEXT("Als.LocomotionAction.GettingUp"))
	UE_DEFINE_GAMEPLAY_TAG(Rolling, TEXT("Als.LocomotionAction.Rolling"))
	UE_DEFINE_GAMEPLAY_TAG(Idle, TEXT("Als.LocomotionAction.Interact"))
	UE_DEFINE_GAMEPLAY_TAG(Interact, TEXT("Als.LocomotionAction.Idle"))
	UE_DEFINE_GAMEPLAY_TAG(Hit, TEXT("Als.LocomotionAction.Hit"))
	UE_DEFINE_GAMEPLAY_TAG(Death, TEXT("Als.LocomotionAction.Death"))
	UE_DEFINE_GAMEPLAY_TAG(Crouch, TEXT("Als.LocomotionAction.Crouch"))
	UE_DEFINE_GAMEPLAY_TAG(Uncrouch, TEXT("Als.LocomotionAction.Uncrouch"))
	UE_DEFINE_GAMEPLAY_TAG(Dodge, TEXT("Als.LocomotionAction.Dodge"))
	UE_DEFINE_GAMEPLAY_TAG(Dodge_F, TEXT("Als.LocomotionAction.Dodge.F"))
	UE_DEFINE_GAMEPLAY_TAG(Dodge_B, TEXT("Als.LocomotionAction.Dodge.B"))
	UE_DEFINE_GAMEPLAY_TAG(Dodge_L, TEXT("Als.LocomotionAction.Dodge.L"))
	UE_DEFINE_GAMEPLAY_TAG(Dodge_R, TEXT("Als.LocomotionAction.Dodge.R"))
	UE_DEFINE_GAMEPLAY_TAG(Dodge_FL, TEXT("Als.LocomotionAction.Dodge.FL"))
	UE_DEFINE_GAMEPLAY_TAG(Dodge_FR, TEXT("Als.LocomotionAction.Dodge.FR"))
	UE_DEFINE_GAMEPLAY_TAG(Dodge_BL, TEXT("Als.LocomotionAction.Dodge.BL"))
	UE_DEFINE_GAMEPLAY_TAG(Dodge_BR, TEXT("Als.LocomotionAction.Dodge.BR"))
	UE_DEFINE_GAMEPLAY_TAG(Step, TEXT("Als.LocomotionAction.Step"))
	UE_DEFINE_GAMEPLAY_TAG(Step_Back, TEXT("Als.LocomotionAction.Step.Back"))
	UE_DEFINE_GAMEPLAY_TAG(Equip, TEXT("Als.LocomotionAction.Equip"))
	UE_DEFINE_GAMEPLAY_TAG(UseItem_01, TEXT("Als.LocomotionAction.UseItem_01"))
	UE_DEFINE_GAMEPLAY_TAG(UseItem_02, TEXT("Als.LocomotionAction.UseItem_02"))
	UE_DEFINE_GAMEPLAY_TAG(UseItem_03, TEXT("Als.LocomotionAction.UseItem_03"))
	UE_DEFINE_GAMEPLAY_TAG(UseItem_04, TEXT("Als.LocomotionAction.UseItem_04"))
	UE_DEFINE_GAMEPLAY_TAG(UseItem_05, TEXT("Als.LocomotionAction.UseItem_05"))
	UE_DEFINE_GAMEPLAY_TAG(UseItem_06, TEXT("Als.LocomotionAction.UseItem_06"))
	UE_DEFINE_GAMEPLAY_TAG(Interact_01, TEXT("Als.LocomotionAction.Interact_01"))
	UE_DEFINE_GAMEPLAY_TAG(Interact_02, TEXT("Als.LocomotionAction.Interact_02"))
	UE_DEFINE_GAMEPLAY_TAG(Interact_03, TEXT("Als.LocomotionAction.Interact_03"))
	UE_DEFINE_GAMEPLAY_TAG(Interact_04, TEXT("Als.LocomotionAction.Interact_04"))
	UE_DEFINE_GAMEPLAY_TAG(Interact_05, TEXT("Als.LocomotionAction.Interact_05"))
	UE_DEFINE_GAMEPLAY_TAG(Interact_06, TEXT("Als.LocomotionAction.Interact_06"))
	UE_DEFINE_GAMEPLAY_TAG(Mount, TEXT("Als.LocomotionAction.Mount"))
	UE_DEFINE_GAMEPLAY_TAG(Dismount, TEXT("Als.LocomotionAction.Dismount"))
	UE_DEFINE_GAMEPLAY_TAG(LightAttack, TEXT("Als.LocomotionAction.Attack.LightAttack"))
	UE_DEFINE_GAMEPLAY_TAG(HeavyAttackCharge, TEXT("Als.LocomotionAction.Attack.HeavyAttackCharge"))
	UE_DEFINE_GAMEPLAY_TAG(HeavyAttack, TEXT("Als.LocomotionAction.Attack.HeavyAttack"))
	UE_DEFINE_GAMEPLAY_TAG(SprintAttack, TEXT("Als.LocomotionAction.Attack.SprintAttack"))
	UE_DEFINE_GAMEPLAY_TAG(RollAttack, TEXT("Als.LocomotionAction.Attack.RollAttack"))
	UE_DEFINE_GAMEPLAY_TAG(JumpAttack, TEXT("Als.LocomotionAction.Attack.JumpAttack"))
	UE_DEFINE_GAMEPLAY_TAG(AirLightAttack, TEXT("Als.LocomotionAction.Attack.AirLightAttack"))
	UE_DEFINE_GAMEPLAY_TAG(AirHeavyAttack, TEXT("Als.LocomotionAction.Attack.AirHeavyAttack"))
	UE_DEFINE_GAMEPLAY_TAG(Reload, TEXT("Als.LocomotionAction.Reload"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_01, TEXT("Als.LocomotionAction.AI_Attack.Attack_01"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_02, TEXT("Als.LocomotionAction.AI_Attack.Attack_02"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_03, TEXT("Als.LocomotionAction.AI_Attack.Attack_03"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_04, TEXT("Als.LocomotionAction.AI_Attack.Attack_04"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_05, TEXT("Als.LocomotionAction.AI_Attack.Attack_05"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_06, TEXT("Als.LocomotionAction.AI_Attack.Attack_06"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_07, TEXT("Als.LocomotionAction.AI_Attack.Attack_07"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_08, TEXT("Als.LocomotionAction.AI_Attack.Attack_08"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_09, TEXT("Als.LocomotionAction.AI_Attack.Attack_09"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_10, TEXT("Als.LocomotionAction.AI_Attack.Attack_10"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_11, TEXT("Als.LocomotionAction.AI_Attack.Attack_11"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_12, TEXT("Als.LocomotionAction.AI_Attack.Attack_12"))
   	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_13, TEXT("Als.LocomotionAction.AI_Attack.Attack_13"))
   	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_14, TEXT("Als.LocomotionAction.AI_Attack.Attack_14"))
   	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_15, TEXT("Als.LocomotionAction.AI_Attack.Attack_15"))
   	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_16, TEXT("Als.LocomotionAction.AI_Attack.Attack_16"))    	
   	UE_DEFINE_GAMEPLAY_TAG(AI_Attack_17, TEXT("Als.LocomotionAction.AI_Attack.Attack_17"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_18, TEXT("Als.LocomotionAction.AI_Attack.Attack_18"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_19, TEXT("Als.LocomotionAction.AI_Attack.Attack_19"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_20, TEXT("Als.LocomotionAction.AI_Attack.Attack_20"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_21, TEXT("Als.LocomotionAction.AI_Attack.Attack_21"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_22, TEXT("Als.LocomotionAction.AI_Attack.Attack_22"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_23, TEXT("Als.LocomotionAction.AI_Attack.Attack_23"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_24, TEXT("Als.LocomotionAction.AI_Attack.Attack_24"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_25, TEXT("Als.LocomotionAction.AI_Attack.Attack_25"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_26, TEXT("Als.LocomotionAction.AI_Attack.Attack_26"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_27, TEXT("Als.LocomotionAction.AI_Attack.Attack_27"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_28, TEXT("Als.LocomotionAction.AI_Attack.Attack_28"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_29, TEXT("Als.LocomotionAction.AI_Attack.Attack_29"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_30, TEXT("Als.LocomotionAction.AI_Attack.Attack_30"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_31, TEXT("Als.LocomotionAction.AI_Attack.Attack_31"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_32, TEXT("Als.LocomotionAction.AI_Attack.Attack_32"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_33, TEXT("Als.LocomotionAction.AI_Attack.Attack_33"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_34, TEXT("Als.LocomotionAction.AI_Attack.Attack_34"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_35, TEXT("Als.LocomotionAction.AI_Attack.Attack_35"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_36, TEXT("Als.LocomotionAction.AI_Attack.Attack_36"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_37, TEXT("Als.LocomotionAction.AI_Attack.Attack_37"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_38, TEXT("Als.LocomotionAction.AI_Attack.Attack_38"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_39, TEXT("Als.LocomotionAction.AI_Attack.Attack_39"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Attack_40, TEXT("Als.LocomotionAction.AI_Attack.Attack_40"))


	
	UE_DEFINE_GAMEPLAY_TAG(AI_Action_01, TEXT("Als.LocomotionAction.AI_Action.Action_01"))
    UE_DEFINE_GAMEPLAY_TAG(AI_Action_02, TEXT("Als.LocomotionAction.AI_Action.Action_02"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Action_03, TEXT("Als.LocomotionAction.AI_Action.Action_03"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Action_04, TEXT("Als.LocomotionAction.AI_Action.Action_04"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Action_05, TEXT("Als.LocomotionAction.AI_Action.Action_05"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Action_06, TEXT("Als.LocomotionAction.AI_Action.Action_06"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Action_07, TEXT("Als.LocomotionAction.AI_Action.Action_07"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Action_08, TEXT("Als.LocomotionAction.AI_Action.Action_08"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Action_09, TEXT("Als.LocomotionAction.AI_Action.Action_09"))
	UE_DEFINE_GAMEPLAY_TAG(AI_Action_10, TEXT("Als.LocomotionAction.AI_Action.Action_10"))
}

namespace AlsGroundedEntryModeTags
{
	UE_DEFINE_GAMEPLAY_TAG(FromRoll, TEXT("Als.GroundedEntryMode.FromRoll"))
}
