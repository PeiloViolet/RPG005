// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include <Engine/DataTable.h>
#include "GenericTeamAgentInterface.h"
#include <GameplayTagContainer.h>
#include "ACFCoreTypes.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class ETeam : uint8
{
	ETeam1   UMETA(DisplayName = "Team 1"),
	ETeam2   UMETA(DisplayName = "Team 2"),
	ETeam3   UMETA(DisplayName = "Team 3"),
	ETeam4   UMETA(DisplayName = "Team 4"),
	ENeutral     UMETA(DisplayName = "Neutral")
};

UENUM(BlueprintType)
enum class EBattleType : uint8
{
	ETeamBased   UMETA(DisplayName = "Team Based"),
	EEveryoneAgainstEveryone	 UMETA(DisplayName = "Everyone Against Everyone"),
};

USTRUCT(BlueprintType)
struct FTeamInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	FTeamInfo():CollisionChannel(ECollisionChannel::ECC_Pawn) {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize, Category = ACF)
	TMap<ETeam, TEnumAsByte<ETeamAttitude::Type>> Relationship;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	TEnumAsByte<ECollisionChannel> CollisionChannel;
};

USTRUCT(BlueprintType)
struct FACFStruct {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
		FGameplayTag TagName;

	FORCEINLINE bool operator!=(const FGameplayTag& Other) const
	{
		return TagName != Other;
	}

	FORCEINLINE bool operator==(const FGameplayTag& Other) const
	{
		return TagName == Other;
	}

};


UCLASS()
class ASCENTCOREINTERFACES_API UACFCoreTypes : public UObject
{
	GENERATED_BODY()
	
};
