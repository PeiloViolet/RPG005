#pragma once
#include "Game/ACFTypes.h"
#include "RPGAIComboActionData.generated.h"


UCLASS(Blueprintable, BlueprintType)
class ASCENTCOMBATFRAMEWORK_API URPGAIComboActionData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	FActionsGroupStruct ConditionActions;
};
