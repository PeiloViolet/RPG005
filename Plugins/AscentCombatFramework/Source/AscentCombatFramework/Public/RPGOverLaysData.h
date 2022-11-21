#pragma once
#include "GameplayTagContainer.h"
#include "Animation/ACFAnimset.h"
#include "RPGOverLaysData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ASCENTCOMBATFRAMEWORK_API URPGOverLaysData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "TagName"), BlueprintReadWrite, Category = "MovesetsOverlays")
	TArray<FAnimationOverlay> Overlays;
};