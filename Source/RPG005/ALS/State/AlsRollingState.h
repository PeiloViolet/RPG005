﻿#pragma once

#include "AlsRollingState.generated.h"

USTRUCT(BlueprintType)
struct RPG005_API FAlsRollingState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = -180, ClampMax = 180, ForceUnits = "deg"))
	float TargetYawAngle{0.0f};
};
