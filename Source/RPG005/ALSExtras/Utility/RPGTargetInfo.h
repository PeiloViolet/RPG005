#pragma once
#include "RPGTargetInfo.generated.h"

USTRUCT(BlueprintType)
struct RPG005_API FRPGTargetInfo
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool HasTarget{false};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Location{0.f,0.f,0.f};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Distance{0.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float X_Y_Distance{0.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator Rotation{0.f,0.f,0.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator LookAtRotation{0.f,0.f,0.f};

};
