// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ACFCurrencyComponent.h"
#include "ACFItemsManagerComponent.h"
#include "ATSTargetingComponent.h"
#include "Groups/ACFCompanionsPlayerController.h"
#include "RPG_Controller.generated.h"

/**
 * 
 */
UCLASS()
class RPG005_API ARPG_Controller : public AACFCompanionsPlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UACFItemsManagerComponent> ItemsManagerComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UATSTargetingComponent> TargetingComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UACFCurrencyComponent> CurrencyComponent;

public:
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputMappingContext> InputMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> TargetAction;
*/
public:
	ARPG_Controller();

protected:
//	virtual void SetupPlayerInputComponent(UInputComponent* Input);

	//void InputTarget();

};
