// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Game/ACFTypes.h"
#include "ACFActivateDamageNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFActivateDamageNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()


protected:


	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	public:

	UPROPERTY(EditAnywhere, Category = ACF)
	EDamageActivationType DamageToActivate;

	UPROPERTY(EditAnywhere, Category = ACF)
	TArray<FName> TraceChannels;
	
};
