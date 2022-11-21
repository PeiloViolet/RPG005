#pragma once
#include "GameplayTagContainer.h"
#include "Game/ACFTypes.h"
#include "RPGCommonActionsData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ASCENTCOMBATFRAMEWORK_API URPGCommonActionsData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "TagName"), BlueprintReadWrite, Category = "CommonActions")
	TMap<FGameplayTag, FActionState> CommonActions;
};