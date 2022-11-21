#pragma once
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "RPG_SetLocomotionActionAndExit_Notify.generated.h"

UCLASS(DisplayName = "RPG Set Locomotion Action And Exit Animation Notify State")
class RPG005_API URPG_SetLocomotionActionAndExit_Notify : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (AllowPrivateAccess))
	FGameplayTag LocomotionAction;

public:
	URPG_SetLocomotionActionAndExit_Notify();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
							 float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
						   const FAnimNotifyEventReference& EventReference) override;
};
