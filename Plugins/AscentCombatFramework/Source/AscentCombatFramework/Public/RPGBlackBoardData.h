#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardData.h"
#include "RPGBlackBoardData.generated.h"

UCLASS()
class ASCENTCOMBATFRAMEWORK_API URPGBlackBoardData : public UBlackboardData
{
	GENERATED_BODY()
public:
	URPGBlackBoardData();
	
	FBlackboardEntry AIState;
	FBlackboardEntry CombatState;
	FBlackboardEntry ComboCounter;
	FBlackboardEntry SelfActor;
	FBlackboardEntry TargetActor;
	FBlackboardEntry TargetLocation;
	FBlackboardEntry TargetActorDistance;
	FBlackboardEntry TargetLocationDistance;
	FBlackboardEntry CommandDuration;
	FBlackboardEntry GroupLeader;
	FBlackboardEntry GroupLeadDistance;
	FBlackboardEntry Paused;
public:
	virtual  void PostInitProperties() override;
	virtual  void PostLoad() override;
};
