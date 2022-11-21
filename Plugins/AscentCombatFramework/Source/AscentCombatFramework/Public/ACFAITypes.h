// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include <Engine/DataTable.h>
#include <GameplayTagContainer.h>
#include "Actors/ACFCharacter.h"
#include "ACFActionCondition.h"
#include "ACFAITypes.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FPatrolConfig
{
	GENERATED_BODY()

public:

	/*If true once the need of the path is reached, those AIs will try to come
	back to the original point */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	bool ReversePath = true;

	/*Amount of time this AI will stay at the patrol point*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	float WaitTimeAtPatrolPoint = 0.f;

};


UENUM(BlueprintType)
enum class EAIState : uint8
{
	EPatrol = 0			UMETA(DisplayName = "Patrol"),
	EBattle = 1 	    UMETA(DisplayName = "Combat"),
	EFollowLeader = 2	UMETA(DisplayName = "Go Home"),
	EWait = 3           UMETA(DisplayName = "Wait"),
};

UENUM(BlueprintType)
enum class EAICombatState : uint8
{
	EMeleeCombat = 0         UMETA(DisplayName = "Melee Attacks"),
	EMeleePostAttack = 1     UMETA(DisplayName = "Melee Post Attacks"),
	ERangedCombat = 2		 UMETA(DisplayName = "Ranged Attacks"),
	EStudyTarget = 3         UMETA(DisplayName = "Studytarget"),
	EFlee = 4                UMETA(DisplayName = "Flee Away"),
	EChaseTarget = 5       UMETA(DisplayName = "Chase Target"),
};


USTRUCT(BlueprintType)
struct FAIAgentsInfo
{
	GENERATED_BODY()

public:

	FAIAgentsInfo()
	{
		AICharacter = nullptr;
		AIController = nullptr;
		//characterClass = AACFCharacter::StatiClass();
	};

	FAIAgentsInfo(class AACFCharacter* inChar, class AACFAIController* inContr)
	{
		AICharacter = inChar;
		AIController = inContr;
		//characterClass = AACFCharacter::StatiClass();
	}

	FAIAgentsInfo(class AACFCharacter* inChar)
	{
		AICharacter = inChar;
	}

	FORCEINLINE	bool operator==(const FAIAgentsInfo& Other) const
	{
		return this->AICharacter == Other.AICharacter;
	}

	FORCEINLINE	bool operator!=(const FAIAgentsInfo& Other) const
	{
		return this->AICharacter != Other.AICharacter;
	}

	FORCEINLINE	bool operator==(const AACFCharacter* Other) const
	{
		return this->AICharacter == Other;
	}

	FORCEINLINE	bool operator!=(const AACFCharacter* Other) const
	{
		return this->AICharacter != Other;
	}


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	AACFCharacter* AICharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	AACFAIController* AIController;

	UPROPERTY(SaveGame)
	TSubclassOf<AACFCharacter> characterClass;

	UPROPERTY(SaveGame)
	FString Guid;
};


USTRUCT(BlueprintType)
struct FCommandChances : public FTableRowBase
{
	GENERATED_BODY()

public:
	FCommandChances()
	{
		Condition = nullptr;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	FGameplayTag CommandTag;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = ACF)
	UACFActionCondition* Condition;

};

USTRUCT(BlueprintType)
struct FAICombatStateConfig
{
	GENERATED_BODY()

public:


	FAICombatStateConfig(): Conditions({NULL}) 
	{
		Conditions.Add(NewObject<UACFDistanceActionCondition>());
		CombatState = EAICombatState::EMeleeCombat;
	};

	FAICombatStateConfig(const EAICombatState inCombatState, float inChance, const ELocomotionState& inState)
	{
		TriggerChancePercentage = inChance;
		LocomotionState = inState;
		Conditions.Add(NewObject<UACFDistanceActionCondition>());
		CombatState = inCombatState;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	EAICombatState CombatState;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = ACF)
	TArray<class UACFActionCondition*> Conditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	float TriggerChancePercentage = 75.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	ELocomotionState LocomotionState = ELocomotionState::EWalk;

	UACFDistanceActionCondition* GetDistanceBasedCondition() const
	{
		for (const auto condition : Conditions)
		{
			UACFDistanceActionCondition* distanceCond = Cast< UACFDistanceActionCondition>(condition);
			if (distanceCond)
			{
				return distanceCond;
			}
		}
		return nullptr;
	}



	FORCEINLINE	bool operator==(const EAICombatState& Other) const
	{
		return this->CombatState == Other;
	}

	FORCEINLINE	bool operator!=(const EAICombatState& Other) const
	{
		return this->CombatState != Other;
	}

	FORCEINLINE	bool operator==(const FAICombatStateConfig& Other) const
	{
		return this->CombatState == Other.CombatState;
	}

	FORCEINLINE	bool operator!=(const FAICombatStateConfig& Other) const
	{
		return this->CombatState != Other.CombatState;
	}


};





UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFAITypes : public UObject
{
	GENERATED_BODY()
};
