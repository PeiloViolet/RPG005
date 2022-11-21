// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actions/ACFBaseAction.h"
#include "ACFUseItemAction.generated.h"

/**
 * 
 */
UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFUseItemAction : public UACFBaseAction
{
	GENERATED_BODY()

protected: 

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	FGameplayTag ItemSlot;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	bool bShouldUseIfInterrupted = false;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	bool bTryToEquipOffhand = false;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bTryToEquipOffhand"), Category = ACF)
	FGameplayTag OffHandSlot;

	bool bSuccess = false;

	virtual void OnActionStarted_Implementation() override;

	virtual void OnNotablePointReached_Implementation() override;

	virtual void OnActionEnded_Implementation() override;

	virtual bool CanExecuteAction_Implementation(class AACFCharacter* owner) override;

private: 
	void UseItem();
	
};
