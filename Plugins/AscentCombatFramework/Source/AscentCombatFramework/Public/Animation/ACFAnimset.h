// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once


#include "CoreMinimal.h"
#include "Game/ACFTypes.h"
#include <GameplayTagContainer.h>
#include "Animation/AnimSequence.h"
#include "ACFAnimset.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FActionsMap : public FTableRowBase {

    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    TMap<FGameplayTag, FActionState> Actions;
};


USTRUCT(BlueprintType)
struct FStopAnimations : public FTableRowBase {

	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	TMap<EACFDirection, UAnimSequence*> StopAnimsMap;
};

USTRUCT(BlueprintType)
struct FACFMoveset : public FACFStruct {

    GENERATED_BODY()

public:
	FACFMoveset():Movement(nullptr), Idle(nullptr),AimOffset(nullptr),ExternalData(nullptr), JumpStart(nullptr),JumpLoop(nullptr),JumpEnd(nullptr),AdditiveBlendspace(nullptr),bUseLeaning(false),AdditiveLeaning(nullptr),bUseStopTransition(false){};
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Movement")
    class UBlendSpace* Movement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Movement")
    class UBlendSpace1D* Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Movement")
	float TurnSmoothingRate = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | AimOffset")
    class UAimOffsetBlendSpace* AimOffset;

	/*PUT HERE EXTERNAL DATA YOU MAY NEED */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Movement")
	UDataAsset* ExternalData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ACF | AimOffset")
	float AimOffsetAlpha = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Jump")
    class UAnimSequence* JumpStart;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Jump")
    class UAnimSequence* JumpLoop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Jump")
    class UAnimSequence* JumpEnd;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Additives")
    class UAimOffsetBlendSpace1D* AdditiveBlendspace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Additives")
	float AdditiveBlendSpeed = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Additives")
	bool bUseLeaning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseLeaning"), Category = "ACF | Additives")
	float LeanAlfa = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseLeaning"), Category = "ACF | Additives")
	class UBlendSpace* AdditiveLeaning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ACF | Stops")
	bool bUseStopTransition;

	/*Stop transitions are triggered when your character suddenly stops while being ABOVE this speed*/
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseStopTransition"), BlueprintReadWrite, Category = "ACF | Stops")
	float MinSpeedToEnterStopTransition = 240.f;

	/*Stop transitions are triggered when your character suddenly stops while being ABOVE this speed*/
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseStopTransition"), BlueprintReadWrite, Category = "ACF | Stops")
	float MinDecelerationToEnterStopTransition = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bUseStopTransition"), Category = "ACF | Stops")
	TMap<ELocomotionState, FStopAnimations> StopTransitions;

};

USTRUCT(BlueprintType)
struct FAnimationOverlay : public FACFStruct {

    GENERATED_BODY()

public:
	FAnimationOverlay():Animation(nullptr),ExternalData(nullptr){};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	class UAnimSequence* Animation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    float BlendingAlpha = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	float InterpolationSpeed = 1.f;

	/*PUT HERE EXTERNAL DATA YOU MAY NEED */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	UDataAsset* ExternalData;
};
