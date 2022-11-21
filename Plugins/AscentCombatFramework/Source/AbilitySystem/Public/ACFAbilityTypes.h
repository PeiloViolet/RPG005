// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <Engine/DataTable.h>
#include "Abilities/ACFAbility.h"
#include <Templates/SubclassOf.h>
#include "ACFAbilityTypes.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FAbilityConfig : public FTableRowBase
{
	GENERATED_BODY()

public:
	FAbilityConfig():Icon(nullptr){};
	/*Tag to be used as you wish*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
		FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
		FName Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
		class UTexture2D* Icon;

	/*Referenced Ability*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
		TSubclassOf<class UACFAbility> Ability;
};

UCLASS()
class ABILITYSYSTEM_API UACFAbilityTypes : public UObject
{
	GENERATED_BODY()
	
};
