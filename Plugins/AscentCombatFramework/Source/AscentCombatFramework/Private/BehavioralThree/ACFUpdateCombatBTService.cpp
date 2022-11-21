// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "BehavioralThree/ACFUpdateCombatBTService.h"
#include <BehaviorTree/BehaviorTreeComponent.h>
#include "ACFAIController.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <GameFramework/Actor.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Float.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>
#include <Navigation/PathFollowingComponent.h>
#include "Components/ACFCombatBehaviourComponent.h"
#include <AI/NavigationSystemBase.h>
#include <NavigationSystem.h>
#include "Game/ACFFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void UACFUpdateCombatBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* const bbc = OwnerComp.GetBlackboardComponent();

	 aiController = Cast<AACFAIController>(OwnerComp.GetAIOwner());

	if (aiController == nullptr )
	{
		UE_LOG(LogTemp, Warning, TEXT("UACFUpdateCombatBTService get controller failed"));
		return;
	}

	targetActor = aiController->GetTargetActorBK();
	if (targetActor == nullptr)
	{
		aiController->RequestAnotherTarget();
		return;
	}

	CharOwner = Cast<AACFCharacter> (aiController->GetPawn());

	if (CharOwner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPawn() is NULL - UACFUpdateCombatBTService"));
		return;
	}

	UpdateCombat();
	SwitchGait();
}

void UACFUpdateCombatBTService::UpdateCombat()
{
	float DistanceToTarget;
	ACharacter* TargetCharacter = Cast< ACharacter>(targetActor);
	if (TargetCharacter)
	{
		if(aiController)
		{
			//更新角色位置黑板键
			aiController->SetTargetLocationBK(targetActor->GetActorLocation());
		}
		//计算距离
		DistanceToTarget = UACFFunctionLibrary::RPGCalculateDistanceBetweenActors(CharOwner, TargetCharacter ,CalculateDistanceType);
	}
	else
	{
		DistanceToTarget = -1.f;
	}
	//更新距离黑板键
	aiController->SetTargetActorDistanceBK(DistanceToTarget);

	//当距离大于所设定的丢失距离时，设置目标为空
	if (DistanceToTarget > aiController->GetLoseTargetDistance())
	{
		aiController->SetTarget(nullptr);
	}

   combatBehav = aiController->GetCombatBehavior();

	if (combatBehav)
	{
		//根据距离初始化战斗状态
		EAICombatState CombatState = combatBehav->GetBestCombatStateByTargetDistance(DistanceToTarget);

		/*
		switch (CombatState) 
		{
		case EAICombatState::EMeleeCombat:
			if (aiController->GetComboCounterBK() > MaxComboCount && combatBehav->EvaluateCombatState(EAICombatState::EMeleePostAttack))
			{
				SetNewMaxComboCount();
				CombatState = EAICombatState::EMeleePostAttack;
			}
			combatBehav->SourceCombatBehaviourData->DefaultCombatBehaviorType = ECombatBehaviorType::EMelee;
			break;
		}
		*/
		//设置战斗状态
		//if (CombatState != aiController->GetCombatStateBK()) aiController->SetCombatStateBK(CombatState);
	}	
}

void UACFUpdateCombatBTService::WalkTrigger_Implementation()
{
	AACFCharacter* Character = Cast<AACFCharacter>(CharOwner);
	if(Character)
	{
		Character->GetLocomotionComponent()->SetLocomotionState(ELocomotionState::EWalk);
	}
}

void UACFUpdateCombatBTService::JogTrigger_Implementation()
{
	AACFCharacter* Character = Cast<AACFCharacter>(CharOwner);
	if(Character)
	{
		Character->GetLocomotionComponent()->SetLocomotionState(ELocomotionState::EJog);
	}
}

void UACFUpdateCombatBTService::SprintTrigger_Implementation()
{
	AACFCharacter* Character = Cast<AACFCharacter>(CharOwner);
	if(Character)
	{
		Character->GetLocomotionComponent()->SetLocomotionState(ELocomotionState::ESprint);
	}
}

void UACFUpdateCombatBTService::SwitchGait_Implementation()
{
	if(!CanSwitchGait) return;
	
	if (aiController && CharOwner)
	{
		AACFCharacter* Character = Cast<AACFCharacter>(CharOwner);
		if(!targetActor) return;
		
		AACFCharacter* TargetCharacter = Cast< AACFCharacter>(targetActor);
		
		if(!Character || !TargetCharacter) return;
		
		float WalkSpeed = Character->GetWalkSpeed();
		float JogSpeed = Character->GetJogSpeed();
		float SprintSpeed = Character->GetSprintSpeed();
		float CurrentSpeed = UKismetMathLibrary::VSize(Character->GetVelocity());
		float Distance = aiController->GetTargetActorDistanceBK();
		
		if(Distance/JogSpeed > SprintChaseTargetTriggerTime)
		{
			SprintTrigger();
			return;
		}
		
		if(Distance < SprintChaseMinTriggerDistance)
		{
			if(Distance/WalkSpeed < WalkChaseTargetTriggerTime && TargetCharacter->GetLocomotionComponent()->GetCurrentLocomotionState() == ELocomotionState::EIdle)
			{
				WalkTrigger();
				return;
			}
			else
			{
				JogTrigger();
				return;
			}
		}
		//JogTrigger();
	}
}

void UACFUpdateCombatBTService::SetNewMaxComboCount()
{
	MaxComboCount = FMath::RandRange(combatBehav->SourceCombatBehaviourData->MinMeleeComboAttacks, combatBehav->SourceCombatBehaviourData->MaxMeleeComboAttacks);
}
