#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "RPG005/ALSExtras/RPGCharacter.h"
#include "RPGNotifyUpdateRotateInfoWithLocked.generated.h"

UCLASS()
class RPG005_API URPGNotifyUpdateRotateInfoWithLocked : public UAnimNotifyState
{
	GENERATED_BODY()
private:
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess))
	ARPGCharacter* Character;

public:
	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
