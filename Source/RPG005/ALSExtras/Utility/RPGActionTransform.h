#pragma once
#include "GameplayTagContainer.h"
#include "RPGActionTransform.generated.h"

USTRUCT(BlueprintType)
struct RPG005_API FRPGActionTransform
{
	GENERATED_BODY()
	FRPGActionTransform():TargetYawAngle(0.f),Curve(nullptr), DistanceRotationCurve(nullptr){};
	//转向总开关
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRefreshRPGActionTransform = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsRefreshRPGActionRotation = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsRefreshRPGActionRotationWithLocked= false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RootMotionCurveRate {70};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator InputRotation{0.f,0.f,0.f};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator StartRotation{0.f,0.f,0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetYawAngle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float TimeSpent{0.f};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentTime{0.f};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Alpha{0.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGameplayTag CurrentAnimActionTag;
	
	//固定时长下的的旋转速率曲线(X,Y,范围为 0 - 1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* Curve;
	
	//有锁翻滚时，根据距离设定的旋转速率曲线
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* DistanceRotationCurve;
};
