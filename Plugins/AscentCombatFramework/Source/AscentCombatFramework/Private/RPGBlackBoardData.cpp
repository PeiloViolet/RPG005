#include "RPGBlackBoardData.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"


URPGBlackBoardData::URPGBlackBoardData()
{
	
	
}

void URPGBlackBoardData::PostInitProperties()
{
	Super::PostInitProperties();
	
}

void URPGBlackBoardData::PostLoad()
{
	Super::PostLoad();

	AIState.EntryName = FName(TEXT("AIState"));
	UBlackboardKeyType_Enum* AIStateEnum= NewObject<UBlackboardKeyType_Enum>();
	AIStateEnum->EnumType = FindObject<UEnum>(ANY_PACKAGE, *FString(TEXT("EAIState")), true);
	AIStateEnum->EnumName = FString(TEXT("AIState"));
	AIState.KeyType = AIStateEnum;
	Keys.Add(AIState);

	CombatState.EntryName = FName(TEXT("CombatState"));
	UBlackboardKeyType_Enum* CombatStateEnum= NewObject<UBlackboardKeyType_Enum>();
	CombatStateEnum->EnumType = FindObject<UEnum>(ANY_PACKAGE, *FString(TEXT("EAICombatState")), true);
	CombatStateEnum->EnumName = FString(TEXT("CombatState"));
	CombatState.KeyType = CombatStateEnum;
	Keys.Add(CombatState);

	ComboCounter.EntryName = FName(TEXT("ComboCounter"));
	ComboCounter.KeyType =  NewObject<UBlackboardKeyType_Int>();
	Keys.Add(ComboCounter);
	
	SelfActor.EntryName = FName(TEXT("SelfActor"));
	SelfActor.KeyType =  NewObject<UBlackboardKeyType_Object>();
	Keys.Add(SelfActor);

	TargetActor.EntryName = FName(TEXT("TargetActor"));
	TargetActor.KeyType =  NewObject<UBlackboardKeyType_Object>();
	Keys.Add(TargetActor);

	TargetLocation.EntryName = FName(TEXT("TargetLocation"));
	TargetLocation.KeyType =  NewObject<UBlackboardKeyType_Vector>();
	Keys.Add(TargetLocation);

	TargetActorDistance.EntryName = FName(TEXT("TargetActorDistance"));
	TargetActorDistance.KeyType =  NewObject<UBlackboardKeyType_Float>();
	Keys.Add(TargetActorDistance);
	
	TargetLocationDistance.EntryName = FName(TEXT("TargetLocationDistance"));
	TargetLocationDistance.KeyType =  NewObject<UBlackboardKeyType_Float>();
	Keys.Add(TargetLocationDistance);
	
	CommandDuration.EntryName = FName(TEXT("CommandDuration"));
	CommandDuration.KeyType =  NewObject<UBlackboardKeyType_Float>();
	Keys.Add(CommandDuration);
	
	GroupLeader.EntryName = FName(TEXT("GroupLeader"));
	GroupLeader.KeyType =  NewObject<UBlackboardKeyType_Object>();
	Keys.Add(GroupLeader);

	GroupLeadDistance.EntryName = FName(TEXT("GroupLeadDistance"));
	GroupLeadDistance.KeyType =  NewObject<UBlackboardKeyType_Float>();
	Keys.Add(GroupLeadDistance);

	Paused.EntryName = FName(TEXT("Paused"));
	Paused.KeyType =  NewObject<UBlackboardKeyType_Bool>();
	Keys.Add(Paused);
}
