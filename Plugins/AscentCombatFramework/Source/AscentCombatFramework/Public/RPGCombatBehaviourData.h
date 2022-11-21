#pragma once
#include "ACFAITypes.h"
#include "Game/ACFTypes.h"

#include "RPGCombatBehaviourData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ASCENTCOMBATFRAMEWORK_API URPGCombatBehaviourData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicConfig")
	ECombatBehaviorType DefaultCombatBehaviorType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicConfig")
	EAICombatState DefaultCombatState = EAICombatState::EMeleeCombat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicConfig")
	uint8 MinMeleeComboAttacks = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicConfig")
	uint8 MaxMeleeComboAttacks = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicConfig")
	bool bNeedsWeapon = false;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = bNeedsWeapon), BlueprintReadWrite, Category = "BasicConfig")
	FGameplayTag EquipMeleeAction;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = bNeedsWeapon), BlueprintReadWrite, Category = "BasicConfig")
	FGameplayTag EquipRangedAction;

	UPROPERTY(EditAnywhere, meta = (EditCondition = bNeedsWeapon), BlueprintReadWrite, Category = "BasicConfig")
	FGameplayTag EngagingAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicConfig")
	TArray<ECombatBehaviorType> AllowedBehaviors;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicConfig")
	TArray<FAICombatStateConfig> CombatStatesConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	TMap<EAICombatState, FActionsGroupStruct> ActionByCombatState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	FActionsGroupStruct ActionByCondition;

	URPGCombatBehaviourData();
};
