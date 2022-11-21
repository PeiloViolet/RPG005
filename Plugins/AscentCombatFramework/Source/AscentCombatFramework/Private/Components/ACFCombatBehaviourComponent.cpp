// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Components/ACFCombatBehaviourComponent.h"
#include "Actors/ACFCharacter.h"
#include "ACFAIController.h"
#include "Components/ACFEquipmentComponent.h"
#include "Game/ACFTypes.h"
#include "ACFActionCondition.h"
#include "Game/ACFFunctionLibrary.h"
//构造函数（组件生成时调用）
UACFCombatBehaviourComponent::UACFCombatBehaviourComponent()
{
	if(SourceCombatBehaviourData)
	{
		
		//this->ActionsByCombatState = SourceCombatBehaviourData->ActionByCombatState;
	}
}
//AI:以State释放Action
bool UACFCombatBehaviourComponent::RPGExecuteActionByCombatState(EAICombatState CombatState)
{
	//检测是否装备武器
	if (!CheckEquipment())
	{
		TryEquipWeapon();
		return false;
	}
	/*
	//检测ActionsByCombatState是否有效
	if(!ActionsByCombatState.Find(CombatState))
	{
		
		return false;
	}
*/
	//检测ActionsByCombatState是否有效
	if(!SourceCombatBehaviourData->ActionByCombatState.Find(CombatState))
	{
		return false;
	}
	
	//按CombatState查找对应的技能列表
	auto CombatStateActions = SourceCombatBehaviourData->ActionByCombatState.Find(CombatState);

	//检测查找到的技能列表是否有效
	if(!CombatStateActions) {return false;}

	if(ExecuteRandomAction(*CombatStateActions))
	{
		return  true;
	}
	return false;
}
//AI:释放下一连招
void UACFCombatBehaviourComponent::RPGExecuteAINextCombo(FActionsGroupStruct CombatActions)
{
	if (!CheckEquipment())
	{
		TryEquipWeapon();
	}
	ExecuteRandomAction(CombatActions);
}
//AI:根据ActionsGroupStruct释放技能
void UACFCombatBehaviourComponent::RPGExecuteActionByActionsGroupStruct(FActionsGroupStruct CombatActions)
{
	if (!CheckEquipment())
	{
		TryEquipWeapon();
	}
	ExecuteRandomAction(CombatActions);
}
//AI:释放技能组内的随机技能
bool UACFCombatBehaviourComponent::ExecuteRandomAction(FActionsGroupStruct CombatActions)
{
	//新建函数变量，并初始化  “技能比例总和”
	float Summation{0.f};
	
	//新加函数变量，并初始化 “允许技能（数组）”
	TArray<FActionStruct> AllowAction{ };
	
	//遍历 “允许技能（数组）” 中的所有技能
	for(const auto Elem : CombatActions.ActionsGroup)
	{
		//检测技能释放需求
		if(ActionRequirements(Elem))
		{
			//将符合要求的添加进入 “允许技能（数组）”
			AllowAction.AddUnique(Elem);
			//“技能比率总和” = “技能比率总和” + “概率比例” × “概率比例权重” (累加）
			Summation = Summation + Elem.ProbabilityProportion * Elem.ProbabilityProportionweight;
		}
	}
	
	//检测 “技能比例总和”
	if(Summation <= 0.f)
	{
		//如果 “技能比例总和” <= 0 则返回 false
		return false;
	}
	
	//新加函数变量，并初始化 “判定随机数”
	//在0 - “技能比例总和” 范围内取随机数
	const float DecideRandomNumber = FMath::RandRange(0.f, Summation);

	//新建函数变量，并初始化  “概率比例和”
	float SummationProbabilityProportion{0.f};

	//遍历 “允许技能（数组）” 中的所有技能
	for (const auto Elem : AllowAction)
	{
		//“概率比例和” = “概率比例和” + “概率比例” × “概率比例权重” (累加）
		SummationProbabilityProportion = SummationProbabilityProportion + Elem.ProbabilityProportion * Elem.ProbabilityProportionweight;
		//如果在遍历过程中 “概率比例和” > “判定随机数”
		if(SummationProbabilityProportion > DecideRandomNumber)
		{
			//触发技能
			if(characterOwner->GetActionsComponent()->CanExecuteAction(Elem.ActionTag))
			{
				aiController->SetWaitDurationTimeBK(Elem.BTWaitTime);
				characterOwner->TriggerAction(Elem.ActionTag, Elem.Priority);
				return true;
			}
		}
	}
	return false;
}
//AI:尝试随机释放ActionByCondition
bool UACFCombatBehaviourComponent::TryExecuteConditionAction()
{
	if (!CheckEquipment())
	{
		TryEquipWeapon();
		return false;
	}
	if(!SourceCombatBehaviourData)
	{
		return false;
	}
	if(ExecuteRandomAction(SourceCombatBehaviourData->ActionByCondition))
	{
		return true;
	}

	return false;
}
//AI:检测技能释放需求
bool UACFCombatBehaviourComponent::ActionRequirements(FActionStruct Action)
{
	//判断允许方向
	if((uint8)characterOwner->GetRelativeTargetDirection(characterOwner->GetTarget()) == (uint8)Action.AllowedFromDirection ||Action.AllowedFromDirection == EActionDirection::EveryDirection)
	{
		//如果使用技能条件
		if(Action.UseCondition)
		{
			//判断条件是否成立
			if(ConditionalValidation(Action))
			{
				return true;
			}
		}
		else 
		{
			//如果条件不存在
			return true;
		}
	}
	return false;
}
//AI:判断条件是否成立
bool UACFCombatBehaviourComponent::ConditionalValidation(const FActionStruct& Action)
{
	if(!Action.Condition)
	{
		//如果技能条件不存在，返回 true
		return true;
	}
	else
	{
		//如果技能条件存在，且 技能条件允许，返回 true，否则返回 false
		return Action.Condition->IsConditionMet(characterOwner);
	}
	return true;
	
}
//
bool UACFCombatBehaviourComponent::IsTargetInMeleeRange( AActor* Target )
{
	if(!SourceCombatBehaviourData)
	{
		return false;
	}
	//检查是否有配置
	const FAICombatStateConfig *ConfigMeleeDistance = SourceCombatBehaviourData->CombatStatesConfig.FindByKey(EAICombatState::EMeleeCombat);
	
	const ACharacter* TargetCharacter = Cast<ACharacter>(Target);
	const float MeleeDistance = GetIdealDistanceByCombatState(EAICombatState::EMeleeCombat);
	if (ConfigMeleeDistance)
	{
		if (TargetCharacter)
		{
			const float Distance = UACFFunctionLibrary::CalculateDistanceBetweenCharactersCapsules(characterOwner, TargetCharacter);
			return MeleeDistance >= Distance;
		}
		if(Target)
		{
			return characterOwner->GetDistanceTo(Target) <= MeleeDistance;
		}
	}		
	return false;
}

EAICombatState UACFCombatBehaviourComponent::GetBestCombatStateByTargetDistance(float targetDistance)
{
	if(!SourceCombatBehaviourData)
	{
		return EAICombatState::EMeleeCombat;
	}
	if (targetDistance == 0) {
		characterOwner->TriggerAction(SourceCombatBehaviourData->EngagingAction, EActionPriority::EMedium);
	}
	for (const auto state : SourceCombatBehaviourData->CombatStatesConfig)
	{
		if (EvaluateCombatState(state.CombatState))
		{
			return state.CombatState;
		}
	}
	return SourceCombatBehaviourData->DefaultCombatState;
}

float UACFCombatBehaviourComponent::GetIdealDistanceByCombatState(EAICombatState combatState) const
{
	if(!SourceCombatBehaviourData)
	{
		return 0.f;
	}
	const FAICombatStateConfig* aiState = SourceCombatBehaviourData->CombatStatesConfig.FindByKey(combatState);
	if (aiState)
	{
		const UACFDistanceActionCondition* distanceCond = aiState->GetDistanceBasedCondition();
		if (distanceCond) {
			return distanceCond->GetDistance();
		}	
	}

	UE_LOG(LogTemp, Warning, TEXT("Unallowed Combat State! - UACFCombatBehaviourComponent::GetIdealDistanceByCombatState"));
	return -1.f;
}

void UACFCombatBehaviourComponent::InitBehavior(class AACFAIController* _controller)
{
	if (_controller)
	{
		aiController = _controller;

		characterOwner = Cast<AACFCharacter>(_controller->GetPawn());

		if (characterOwner && characterOwner->GetLocomotionComponent())
		{
			characterOwner->SetStrafeMovement(characterOwner->GetLocomotionComponent()->IsCharacterStrafing());
		}	

		if (!CheckEquipment())
		{
			TryEquipWeapon();
		}
	}
}

void UACFCombatBehaviourComponent::TryEquipWeapon()
{
	UACFEquipmentComponent* equipComp = characterOwner->GetEquipmentComponent();

	ensure(equipComp);
	
	if(!SourceCombatBehaviourData)
	{
		return ;
	}
	if (SourceCombatBehaviourData->DefaultCombatBehaviorType == ECombatBehaviorType::EMelee)
	{
		characterOwner->TriggerAction(SourceCombatBehaviourData->EquipMeleeAction, EActionPriority::EHigh);
		aiController->SetCombatStateBK(EAICombatState::EMeleeCombat);
	}
	else if (SourceCombatBehaviourData->DefaultCombatBehaviorType == ECombatBehaviorType::ERanged)
	{
		characterOwner->TriggerAction(SourceCombatBehaviourData->EquipRangedAction, EActionPriority::EHigh);
		aiController->SetCombatStateBK(EAICombatState::ERangedCombat);
	}

}

void UACFCombatBehaviourComponent::UninitBehavior()
{

	if(!SourceCombatBehaviourData)
	{
		return ;
	}
	if (SourceCombatBehaviourData->bNeedsWeapon && characterOwner && characterOwner->GetCombatType() != ECombatType::EUnarmed)
	{
		UACFEquipmentComponent* equipComp = characterOwner->GetEquipmentComponent();

		ensure(equipComp);
		FGameplayTag unequipAction = SourceCombatBehaviourData->DefaultCombatBehaviorType == ECombatBehaviorType::EMelee ? SourceCombatBehaviourData->EquipMeleeAction : SourceCombatBehaviourData->EquipRangedAction;
		characterOwner->TriggerAction(unequipAction, EActionPriority::EHighest);
		characterOwner->SetStrafeMovement(false);
	}

}

bool UACFCombatBehaviourComponent::CheckEquipment()
{
	if(!SourceCombatBehaviourData)
	{
		return false;
	}

	return ((characterOwner && (characterOwner->GetCombatType() == ECombatType::EMelee || characterOwner->GetCombatType() == ECombatType::ERanged))
		     || !SourceCombatBehaviourData->bNeedsWeapon  );
	/*return (characterOwner && characterOwner->GetCombatType() == ECombatType::EMelee &&
		    SourceCombatBehaviourData->bNeedsWeapon &&
		    SourceCombatBehaviourData->DefaultCombatBehaviorType == ECombatBehaviorType::EMelee) ||
		   (characterOwner && characterOwner->GetCombatType() == ECombatType::ERanged && SourceCombatBehaviourData->bNeedsWeapon && SourceCombatBehaviourData->DefaultCombatBehaviorType == ECombatBehaviorType::ERanged) ;

     */
}


bool UACFCombatBehaviourComponent::EvaluateCombatState(EAICombatState combatState)
{
	if(!SourceCombatBehaviourData)
	{
		return false;
	}
	if (!SourceCombatBehaviourData->CombatStatesConfig.Contains(combatState))
	{
		return false;
	}
	const FAICombatStateConfig* CombatState = SourceCombatBehaviourData->CombatStatesConfig.FindByKey(combatState);

	if (CombatState)
	{
		for (auto condition : CombatState->Conditions)
		{
			if (!condition)
			{
				UE_LOG(LogTemp, Error, TEXT("INVALID ACTION CONDITION IN COMBAT CONFIG! - UACFCombatBehaviourComponent"));
				continue;
			}
			if (condition && !condition->IsConditionMet(characterOwner))
			{
				return false;
			}
		}
		return FMath::RandRange(0.f, 100.f) <= CombatState->TriggerChancePercentage;
	}
	return false;
}



// void UACFCombatBehaviourComponent::UpdateBehaviorType()
// {
// 	if (IdealDistanceByCombatState.Contains(ECombatBehaviorType::ERanged))
// 	{	
// 		float* randegdist = IdealDistanceByCombatState.Find(ECombatBehaviorType::ERanged);
// 		if (randegdist &&  aiController->GetTargetActorDistanceBK() > *randegdist)
// 		{
// 			CurrentCombatBehaviorType = ECombatBehaviorType::ERanged;
// 			return;
// 		}			
// 	}
// 	CurrentCombatBehaviorType = ECombatBehaviorType::EMelee;
// }
