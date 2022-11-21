#pragma once
#include "RPGAIComboActionData.h"
#include "ACFAIcontroller.h"
#include "Components/ACFCombatBehaviourComponent.h"
#include "RPGAIComboNotify.generated.h"
UCLASS()
class RPG005_API URPGAIComboNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	
private:
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess))
	UACFCombatBehaviourComponent* CombatBehaviourComponent;
	
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess))
	AACFAIController* Controller;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (AllowPrivateAccess))
	TObjectPtr<URPGAIComboActionData> NextComboActionData = nullptr;;
	
protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
