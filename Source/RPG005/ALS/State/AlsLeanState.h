﻿#pragma once

#include "AlsLeanState.generated.h"

USTRUCT(BlueprintType)
struct RPG005_API FAlsLeanState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -1, ClampMax = 1))
	float RightAmount{0.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -1, ClampMax = 1))
	float ForwardAmount{0.0f};
};
