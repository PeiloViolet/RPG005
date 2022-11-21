// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "ACFMountComponent.h"
#include "ACFRiderComponent.h"
#include "Actors/ACFCharacter.h"
#include "GameFramework/Controller.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/ACFGroupAIComponent.h"

UACFMountComponent::UACFMountComponent()
{
}

// Sets default values for this component's properties
void UACFMountComponent::SetMounted(bool inMounted)
{
    if (GetPawnOwner() && rider) {
        if (bIsPlayerCompanion) {
            const AController* riderContr = rider->GetController();
            UACFGroupAIComponent* groupAi = riderContr->FindComponentByClass<UACFGroupAIComponent>();
            AACFCharacter* acfRiderChar = Cast<AACFCharacter>(GetPawnOwner());
            if (groupAi) {
                if (bIsMounted) {
                    groupAi->RemoveAgentFromGroup(acfRiderChar);
                } else {
                    groupAi->AddExistingCharacterToGroup(acfRiderChar);
                }
            }
        } else {
            const AController* contr = GetPawnOwner()->GetController();
            if (contr) {
                UBehaviorTreeComponent* behavComp = GetPawnOwner()->GetController()->FindComponentByClass<UBehaviorTreeComponent>();
                if (behavComp) {
                    if (inMounted) {
						behavComp->PauseLogic("Mounted");
                    }
                    else {
                        behavComp->ResumeLogic("Mounted");
                    }
                }
            }
    
        }
    }
	Super::SetMounted(inMounted);
}


