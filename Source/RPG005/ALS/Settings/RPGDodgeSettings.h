#pragma once

#include "Animation/AnimMontage.h"
#include "RPGDodgeSettings.generated.h"

USTRUCT(BlueprintType)
struct RPG005_API FRPGDodgeSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotateToInputOnStart{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float RotationInterpolationSpeed{10.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInterruptRollingWhenInAir{true};
};
