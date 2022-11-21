
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Animation/AnimNotifyQueue.h"
#include "RPG005/ALSExtras/RPGCharacter.h"
#include "Components/ACFActionsManagerComponent.h"
#include "RPG005/ALS/AlsAnimationInstance.h"
#include "RPGPlayNextCombo.generated.h"

UCLASS()
class RPG005_API URPGPlayNextCombo : public UAnimNotifyState
{
	GENERATED_BODY()
private:
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