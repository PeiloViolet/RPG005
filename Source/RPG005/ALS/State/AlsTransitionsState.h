#pragma once

#include "AlsTransitionsState.generated.h"

class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct RPG005_API FAlsTransitionsState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTransitionsAllowed{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DynamicTransitionsFrameDelay{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimSequenceBase> QueuedDynamicTransitionAnimation{nullptr};
};
