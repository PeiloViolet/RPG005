#pragma once
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "RPG005/ALSExtras/RPGCharacter.h"
#include "RPGNotifySwitchRootRotation.generated.h"

UCLASS()
class  RPG005_API URPGNotifySwitchRootRotation : public UAnimNotifyState
{
public:
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (AllowPrivateAccess))
	float BlendOutTime{0.3};
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess))
	ARPGCharacter* Character;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess))
	UAlsCharacterMovementComponent* AlsCharacterMovementComponent;

public:
	virtual void NotifyBegin(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Animation,
							 float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation,const FAnimNotifyEventReference& EventReference);
};
