#include "RPG_HeavyAttact.h"




void URPG_HeavyAttact::OnInputActionTriggered_Implementation()
{
	IAttackInterface::OnInputActionTriggered_Implementation();
	//CanSkipCharge = true;
	/*
	UACFAnimInstance* AnimInstance = CharacterOwner->GetACFAnimInstance();
	AnimInstance->Montage_JumpToSectionsEnd(AnimInstance->Montage_GetCurrentSection());
	*/
}
