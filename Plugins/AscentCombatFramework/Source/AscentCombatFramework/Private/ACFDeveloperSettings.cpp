// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "ACFDeveloperSettings.h"
#include "Items/ACFWorldItem.h"
#include "Game/ACFDamageCalculation.h"




UACFDeveloperSettings::UACFDeveloperSettings()
{
	DamageCalculatorClass = UACFDamageCalculation::StaticClass();
}
