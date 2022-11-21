#pragma once
#include "GameplayTagContainer.h"
#include "Animation/ACFAnimset.h"
#include "Game/ACFTypes.h"
#include "RPGMovesetsActionsData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ASCENTCOMBATFRAMEWORK_API URPGMovesetsActionsData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "TagName"), BlueprintReadWrite, Category = "MovesetsActions")
	TMap<FGameplayTag, FActionsMap> MovesetsActions;
};