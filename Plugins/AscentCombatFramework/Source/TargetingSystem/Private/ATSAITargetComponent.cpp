// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "ATSAITargetComponent.h"
#include <AIController.h>
#include "ATSTargetPointComponent.h"
#include <Engine/TargetPoint.h>

void UATSAITargetComponent::SetCurrentTarget(class AActor* target)
{
	Super::SetCurrentTarget(target);

	AAIController* controllerOwner = Cast<AAIController>(GetOwner());
	if (target)
	{

		UATSTargetPointComponent* pointToLookAt = target->FindComponentByClass<UATSTargetPointComponent>();

		if (pointToLookAt)
		{		
			controllerOwner->SetFocus(pointToLookAt->GetTargetPoint(), EAIFocusPriority::Gameplay);
			CurrentTargetPoint = pointToLookAt;
		}
		else
		{
			controllerOwner->SetFocus(target, EAIFocusPriority::Gameplay);
		}
	}
	else
	{

		controllerOwner->ClearFocus(EAIFocusPriority::Gameplay);
	}
}
