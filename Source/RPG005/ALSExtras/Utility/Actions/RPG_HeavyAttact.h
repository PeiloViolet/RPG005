#pragma once
#include "Actions/ACFAttackAction.h"
#include "RPG005/ALSExtras/RPGCharacter.h"
#include "RPG005/ALSExtras/Utility/Interface/AttackInterface.h"
#include "RPG_HeavyAttact.generated.h"

UCLASS()
class RPG005_API URPG_HeavyAttact : public UACFAttackAction, public IAttackInterface
{
	GENERATED_BODY()
public:
	virtual void OnInputActionTriggered_Implementation() override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPG)
	//bool CanSkipCharge{false};
};
