#pragma once

#include "Animation/PoseSnapshot.h"
#include "AlsRagdollingAnimationState.generated.h"

USTRUCT(BlueprintType)
struct RPG005_API FAlsRagdollingAnimationState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPoseSnapshot FinalRagdollPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0, ClampMax = 1, ForceUnits = "x"))
	float FlailPlayRate{1.0f};
};
