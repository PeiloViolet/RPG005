// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "ACFAbilityFunctionLibrary.h"
#include <Engine/DataTable.h>
#include "ACFAbilityManagerComponent.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/GameState.h>




bool UACFAbilityFunctionLibrary::GetAbilityFromDB( const FName& inName, FAbilityConfig& outAbility, const UObject* WorldContextObject)
{
	auto gameState = UGameplayStatics::GetGameState(WorldContextObject);

	if (!gameState) {
		return false;
	}
	UACFAbilityManagerComponent* settings = gameState->FindComponentByClass< UACFAbilityManagerComponent>();
	
	if (settings && settings->AbilitiesDB) {
		FAbilityConfig* ability = settings->AbilitiesDB->FindRow<FAbilityConfig>(inName, "Finding Ability...");
		if (ability) {
			outAbility = *ability;
			return true;
		}
	}
	return false;

}


bool UACFAbilityFunctionLibrary::GetAbilitiesFromDB( const TArray<FName>& inName, TArray<FAbilityConfig>& outAbility, const UObject* WorldContextObject)
{
	bool success = true;
	for (const FName& row : inName) {
		FAbilityConfig toBeAdded; 
		if (GetAbilityFromDB(row, toBeAdded, WorldContextObject)) {
			outAbility.Add(toBeAdded);
		}
		else {
			success = false;
		}
		
	}
	return success;
}
