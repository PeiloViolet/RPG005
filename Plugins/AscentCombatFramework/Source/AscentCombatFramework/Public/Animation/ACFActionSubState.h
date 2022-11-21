// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ACFActionSubState.generated.h"

/**
 * 
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFActionSubState : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected: 


	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;
	
public:

// 	UPROPERTY(EditAnywhere)
// 		bool bActivateOnlyOnServer = true;
// 	
};
