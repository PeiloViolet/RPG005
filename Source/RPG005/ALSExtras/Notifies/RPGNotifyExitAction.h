#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Animation/AnimNotifyQueue.h"
#include "RPG005/ALSExtras/RPGCharacter.h"
#include "Components/ACFActionsManagerComponent.h"
#include "RPG005/ALS/AlsAnimationInstance.h"
#include "RPGNotifyExitAction.generated.h"

UCLASS()
class RPG005_API URPGNotifyExitAction : public UAnimNotifyState
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (AllowPrivateAccess))
	float BlendOutTime{0.3};
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess))
	bool CanTick{false};
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess))
	ARPGCharacter* Character;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess))
	UAlsAnimationInstance* AlsAnimationInstance;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess))
	UACFActionsManagerComponent* ActionsManagerComponent;
public:
	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference);
};
