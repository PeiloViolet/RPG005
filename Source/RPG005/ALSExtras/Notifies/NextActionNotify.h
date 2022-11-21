#pragma once
#include "GameplayTagContainer.h"
#include "Actors/ACFCharacter.h"


#include "NextActionNotify.generated.h"

UCLASS()
class RPG005_API UNextActionNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (AllowPrivateAccess))
	FGameplayTag ActionTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", Meta = (AllowPrivateAccess))
	EActionPriority Priority;
	
protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};