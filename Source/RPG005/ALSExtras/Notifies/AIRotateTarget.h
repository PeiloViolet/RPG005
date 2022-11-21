#pragma once
#include "CoreMinimal.h"
#include "Actors/ACFCharacter.h"
#include "ACFAIController.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Animation/AnimNotifyQueue.h"
#include "AIRotateTarget.generated.h"


UCLASS()
class  RPG005_API UAIRotateTarget : public UAnimNotifyState
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (AllowPrivateAccess))
	float InlerpSpeed{5};
	
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess))
	AACFCharacter* Character{nullptr};

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess))
	AACFCharacter* TargetCharacter{nullptr};
	
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess))
	AACFAIController* AIController{nullptr};
	

public:
	UAIRotateTarget();
	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference);
};
