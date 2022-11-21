// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "ACFActionCondition.h"
#include "BehaviorTree/BTService.h"
#include "Game/ACFTypes.h"
#include "ACFAITypes.h"
#include "ACFUpdateCombatBTService.generated.h"

/**
 * 
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFUpdateCombatBTService : public UBTService
{
	GENERATED_BODY()
	
protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void SetNewMaxComboCount();

	void UpdateCombat();

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	class AACFAIController*  aiController;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
		class AActor*  targetActor;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
		class  AACFCharacter*  CharOwner;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
		class UACFCombatBehaviourComponent* combatBehav;

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RPG)
	CalculateDistanceType CalculateDistanceType{CalculateDistanceType::Capsules};

	UPROPERTY(BlueprintReadOnly, Category = ACF)
	bool CanSwitchGait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPG)
	float SprintChaseMaxTriggerDistance = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPG)
	float SprintChaseMinTriggerDistance = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPG)
	float SprintChaseTargetTriggerTime = 1.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPG)
	float WalkChaseTargetTriggerTime = 3.f;
public: 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	FGameplayTag SwitchCombatTypeActionTag ;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = RPG)
	void SwitchGait();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = RPG)
	void SprintTrigger();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = RPG)
	void JogTrigger();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = RPG)
	void WalkTrigger();


};
