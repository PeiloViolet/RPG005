#pragma once
#include "Animation/ACFAnimset.h"
#include "Game/ACFTypes.h"
#include "RPGMovesetsData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ASCENTCOMBATFRAMEWORK_API URPGMovesetsData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (TitleProperty = "TagName"), BlueprintReadWrite, Category = "Movesets")
	TArray<FACFMoveset> Movesets;
};