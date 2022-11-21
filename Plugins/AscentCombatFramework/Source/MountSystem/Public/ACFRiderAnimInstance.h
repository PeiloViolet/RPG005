// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Animation/ACFAnimInstance.h"
#include "ACFRiderAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MOUNTSYSTEM_API UACFRiderAnimInstance : public UACFAnimInstance
{
	GENERATED_BODY()
public: 

	/* begin play */
	virtual void NativeInitializeAnimation() override;
	/* tick */
	virtual void NativeUpdateAnimation(float DeltaTime) override;


protected:

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	ECombatType CombatType;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	bool bIsRiding;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	float MountSpeed;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	float MountTurn;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	class UACFRiderComponent* RiderComp;

private:

	UFUNCTION()
	void HandleCombatTypeChanged(ECombatType inCombatType);

	UFUNCTION()
	void HandleRidingChanged(bool inIsRiding);
	
};
