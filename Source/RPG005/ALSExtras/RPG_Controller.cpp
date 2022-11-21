// Fill out your copyright notice in the Description page of Project Settings.
#include "RPG_Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


ARPG_Controller::ARPG_Controller()
{
	ItemsManagerComponent = CreateDefaultSubobject<UACFItemsManagerComponent>(TEXT("ItemsManager"));
	TargetingComponent = CreateDefaultSubobject<UATSTargetingComponent>(TEXT("TargetingComponent"));
	CurrencyComponent = CreateDefaultSubobject<UACFCurrencyComponent>(TEXT("CurrencyComponent"));
}
/*
void ARPG_Controller::SetupPlayerInputComponent(UInputComponent* Input)
{
	auto* EnhancedInput{Cast<UEnhancedInputComponent>(Input)};
	if (IsValid(EnhancedInput))
	{
		EnhancedInput->BindAction(TargetAction, ETriggerEvent::Triggered, this, &ThisClass::InputTarget);
	}
}

void ARPG_Controller::InputTarget()
{
	TargetingComponent->ToggleTargeting();
}
*/