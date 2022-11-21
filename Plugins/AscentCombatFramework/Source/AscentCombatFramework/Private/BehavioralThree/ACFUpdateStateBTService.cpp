// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "BehavioralThree/ACFUpdateStateBTService.h"
#include "ACFAIController.h"
#include <BehaviorTree/BehaviorTreeComponent.h>
#include "Components/ACFCommandsManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UACFUpdateStateBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	aiController = Cast<AACFAIController>(OwnerComp.GetAIOwner());	
	if (aiController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UACFUpdateCombatBTService get controller failed"));
		return;
	}


	CharOwner = aiController->GetPawn();
	if (CharOwner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPawn() is NULL - UACFUpdateCombatBTService"));
		aiController->Destroy();
		return;
	}
	const AActor* lead = aiController->GetLeadActorBK();
	if (lead)
	{
		const float distanceFromLeader = (CharOwner->GetActorLocation() - lead->GetActorLocation()).Size();
		aiController->SetHomeDistanceBK(distanceFromLeader);

		if (aiController->ShouldTeleportHome() && distanceFromLeader >= aiController->GetTeleportHomeDistanceTrigger())
			{
			aiController->TeleportNearLead();
			aiController->SetTarget(nullptr);
			return;
		}
		if (aiController->ShouldCameHome() && aiController->GetAIStateBK() != EAIState::EFollowLeader)
		{
			if (distanceFromLeader > aiController->GetMaxDistanceFromLead() )
			{
				aiController->SetCurrentAIStateBK(EAIState::EFollowLeader);
			}
		}
	}

}



