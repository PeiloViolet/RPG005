// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Components/ACFGroupAIComponent.h"
#include "ACFAIController.h"
#include "Components/ACFThreatManagerComponent.h"
#include "Actors/ACFCharacter.h"
#include "Game/ACFFunctionLibrary.h"
#include "Game/ACFPlayerController.h"
#include "Game/ACFTypes.h"
#include <Engine/World.h>
#include <GameFramework/Pawn.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UACFGroupAIComponent::UACFGroupAIComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bInBattle = false;
    SetIsReplicatedByDefault(true);
    spawnOffset.X = 200.f;
    spawnOffset.Y = 250.f;
}

void UACFGroupAIComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UACFGroupAIComponent, groupLead);
}
// Called when the game starts
void UACFGroupAIComponent::BeginPlay()
{
    Super::BeginPlay();
    SetReferences();
}

void UACFGroupAIComponent::SetReferences()
{
    groupLead = Cast<AActor>(GetOwner());
}

void UACFGroupAIComponent::OnComponentLoaded_Implementation()
{
    for (int32 index = 0; index < AICharactersInfo.Num(); index++) {
        FAIAgentsInfo& agent = AICharactersInfo[index];
        TArray<AActor*> foundActors;
        UGameplayStatics::GetAllActorsOfClassWithTag(this, agent.characterClass, FName(*agent.Guid), foundActors);
        if (foundActors.Num() == 0) {
            UE_LOG(LogTemp, Error, TEXT("Impossible to find actor"));
            continue;
        }
        agent.AICharacter = Cast<AACFCharacter>(foundActors[0]);
        agent.AIController = Cast<AACFAIController>(agent.AICharacter->GetController());
        InitAgent(agent, index);
    }
}

void UACFGroupAIComponent::SendCommandToCompanions_Implementation(FGameplayTag command)
{
    Internal_SendCommandToAgents(command);
}

bool UACFGroupAIComponent::SendCommandToCompanions_Validate(FGameplayTag command)
{
    return true;
}

void UACFGroupAIComponent::SpawnGroup_Implementation()
{
    if (bAlreadySpawned && !CanSpawnMultitpleTimes) {
        return;
    }
    Internal_SpawnGroup();
    bAlreadySpawned = true;
}

bool UACFGroupAIComponent::SpawnGroup_Validate()
{
    return true;
}

void UACFGroupAIComponent::DespawnGroup_Implementation(const bool bResetSpawnArray /*= false*/)
{
    if (!bResetSpawnArray) {
        AIToSpawn.Empty();
    }
    if (!bResetSpawnArray) {
        for (FAIAgentsInfo& agent : AICharactersInfo) {
            if (agent.AICharacter && agent.AICharacter->IsAlive()) {
                const TSubclassOf<AACFCharacter> charClass = agent.AICharacter->GetClass();
                AddAIToSpawn(charClass);
                agent.AICharacter->KillCharacter();
            }
        }
    }
    OnAllAgentDeath.Broadcast();
    AICharactersInfo.Empty();
    bAlreadySpawned = false;
}

bool UACFGroupAIComponent::DespawnGroup_Validate(const bool bResetSpawnArray /*= false*/)
{
    return true;
}

void UACFGroupAIComponent::InitAgents()
{
    for (int32 index = 0; index < AICharactersInfo.Num(); index++) {
        if (AICharactersInfo.IsValidIndex(index)) {
            InitAgent(AICharactersInfo[index], index);
        }
    }
}

void UACFGroupAIComponent::InitAgent(FAIAgentsInfo& agent, int32 childIndex)
{
    if (agent.AICharacter && !agent.AIController) {
        agent.AICharacter->SpawnDefaultController();
        agent.AIController = Cast<AACFAIController>(agent.AICharacter->GetController());
    }

    if (agent.AIController && agent.AICharacter) {
        agent.AIController->SetGroupOwner(this, childIndex, bOverrideAgentPerception, bOverrideAgentTeam);
        agent.AIController->SetLeadActorBK(groupLead);
        agent.AIController->SetDefaultState(DefaultAiState);
        agent.AIController->SetCurrentAIStateBK(DefaultAiState);
        agent.AIController->SetPatrolPath(PatrolPath);
        if (bOverrideAgentTeam) {
            agent.AICharacter->AssignTeam(CombatTeam);
        }
        agent.characterClass = (agent.AICharacter->GetClass());
        check(agent.characterClass);
        if (!agent.AICharacter->Tags.Contains(FName(*agent.Guid))) {
            const FString newGuid = FGuid::NewGuid().ToString();
            agent.AICharacter->Tags.Add(FName(*newGuid));
            agent.Guid = newGuid;
        }
    }
}

void UACFGroupAIComponent::AddAIToSpawn(const TSubclassOf<AACFCharacter>& charClass)
{
    if (AIToSpawn.Contains(charClass)) {
        AIToSpawn.FindByKey(charClass)->TotalNumber++;
    } else {
        AIToSpawn.Add(FAISpawnInfo(charClass));
    }
}

void UACFGroupAIComponent::RemoveAIToSpawn(const TSubclassOf<AACFCharacter>& charClass)
{
    if (AIToSpawn.Contains(charClass)) {
        const int32 aiNum = AIToSpawn.FindByKey(charClass)->TotalNumber--;
        if (aiNum == 0) {
            AIToSpawn.Remove(charClass);
        }
    }
}

bool UACFGroupAIComponent::GetAgentByIndex(int32 index, FAIAgentsInfo& outAgent) const
{
    if (AICharactersInfo.IsValidIndex(index)) {
        outAgent = AICharactersInfo[index];
        return true;
    }
    return false;
}

void UACFGroupAIComponent::Internal_SendCommandToAgents(FGameplayTag command)
{
    for (FAIAgentsInfo& achar : AICharactersInfo) {
        if (achar.AIController) {
            achar.AIController->TriggerCommand(command);
        }
    }
}

void UACFGroupAIComponent::SetEnemyGroup(UACFGroupAIComponent* inEnemyGroup)
{
    if (inEnemyGroup && UACFFunctionLibrary::AreEnemyTeams(GetWorld(), GetCombatTeam(), inEnemyGroup->GetCombatTeam())) {
        enemyGroup = inEnemyGroup;
    }
}

FVector UACFGroupAIComponent::GetGroupCentroid() const
{
    TArray<AActor*> actors;
    for (const auto& agent : AICharactersInfo) {
        if (agent.AICharacter) {
            actors.Add(agent.AICharacter);
        }
    }
    return UGameplayStatics::GetActorArrayAverageLocation(actors);
}

class AACFCharacter* UACFGroupAIComponent::RequestNewTarget(const AACFAIController* requestSender)
{
    //First Try to help lead
    const AACFCharacter* lead = Cast<AACFCharacter>(requestSender->GetLeadActorBK());
    if (lead) {
        const AACFCharacter* newTarget = Cast<AACFCharacter>(lead->GetTarget());
        if (newTarget && newTarget->IsMyEnemy(requestSender->GetBaseAICharacter()))
            return Cast<AACFCharacter>(lead->GetTarget());
    }

    //Then Try to help other in  the group
    if (AICharactersInfo.IsValidIndex(0) && AICharactersInfo[0].AICharacter->IsValidLowLevel() && AICharactersInfo[0].AIController->IsValidLowLevel()) {
        for (FAIAgentsInfo achar : AICharactersInfo) {
            if (achar.AIController && achar.AIController != requestSender) {
                AACFCharacter* newTarget = Cast<AACFCharacter>(achar.AIController->GetTargetActorBK());
                if (newTarget && newTarget->IsAlive() && achar.AIController->GetAIStateBK() == EAIState::EBattle) {
                    return newTarget;
                }
            }
        }
    }

    //Then Try to help other in  the group
    if (enemyGroup) {
        return enemyGroup->GetAgentNearestTo(requestSender->GetPawn()->GetActorLocation());
    }

    return nullptr;
}

void UACFGroupAIComponent::Internal_SpawnGroup()
{
    if (AIToSpawn.Num() > 0) {
        const UWorld* world = GetWorld();
        if (world) {
            FActorSpawnParameters spawnParam;
            spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            // offset on forward direction
            float xOffset = spawnOffset.X;
            float yOffset = spawnOffset.Y;

            int32 maxGroupWidth = -1;
            int32 maxGroupLength = -1;

            int32 childGroupIndex = 0;

            // the first row of current class
            int32 fi_currClassRowBase = 0;
            int32 colPadding = 0;
            int32 rowPadding = 0;

            for (auto& aiSpawn : AIToSpawn) {
                const TSubclassOf<AACFCharacter> ClassToSpawn = aiSpawn.AIClassBP;
                int32 Remain = aiSpawn.TotalNumber;
                const int32 maxRowSize = aiSpawn.MaxNumOfCol;
                ensure(maxRowSize > 0);
                colPadding = aiSpawn.ColumnPadding;
                rowPadding = aiSpawn.RowPadding;

                if (Remain == 0) {
                    UE_LOG(LogTemp, Error, TEXT("MaxNumOfCol can not be zero - UACFGroupAIComponent::Internal_SpawnGroup"));
                    return;
                }
                // find out how many row we have for this class
                while (Remain > 0) {
                    int32 temp = Remain - maxRowSize;
                    if (temp > 0) {
                        FormationInfo.Add(maxRowSize);
                        Remain = temp;
                    } else {
                        FormationInfo.Add(Remain);
                        break;
                    }
                }

                const int32 maxRow = FormationInfo.Num() - fi_currClassRowBase;
                int32 totalUnit = 0;
                for (int iRow = 0; iRow < maxRow; iRow++) {
                    const int32 currentRow = fi_currClassRowBase + iRow;
                    const int32 currentRowSize = FormationInfo[currentRow];
                    const float rowWidth = (currentRowSize - 1) * colPadding;
                    if (rowWidth > maxGroupWidth) {
                        maxGroupWidth = rowWidth;
                    }

                    const float centerOffset = 0.5f * rowWidth;
                    for (int32 iCol = 0; iCol < currentRowSize && totalUnit < aiSpawn.TotalNumber; iCol++) {
                        yOffset = iCol * colPadding - centerOffset;
                        // Spawn
                        childGroupIndex = AddAgentToGroup(xOffset, yOffset, ClassToSpawn);
                        totalUnit++;
                        if (childGroupIndex == -1) {
                            return;
                        }
                    }
                    xOffset += rowPadding;
                }
                fi_currClassRowBase += maxRow;
                aiSpawn.NumOfRow = maxRow;
            }
        }
        bAlreadySpawned = true;
        OnAgentsSpawned.Broadcast();
    } else {
        UE_LOG(LogTemp, Error, TEXT("%s NO AI to Spawn - AAIFGROUPPAWN::SpawnAGroup"), *this->GetName());
    }
}

uint8 UACFGroupAIComponent::AddAgentToGroup(float inXOffset, float inYOffset, const TSubclassOf<AACFCharacter>& ClassToSpawn)
{
    UWorld* const world = GetWorld();

    ensure(GetOwner()->HasAuthority());

    if (!world)
        return -1;

    if (!groupLead) {
        SetReferences();
        if (!groupLead) {
            return -1;
        }
    }

    if (AICharactersInfo.Num() >= MaxSimultaneousAgents) {
        return -1;
    }

    FAIAgentsInfo newCharacterInfo;

    const FVector myLocation = GetComponentLocation();
    const FVector spawnLocation = myLocation - inXOffset * groupLead->GetActorForwardVector() + inYOffset * groupLead->GetActorRightVector();
    FTransform spawnTransform;
    spawnTransform.SetLocation(spawnLocation);
    spawnTransform.SetRotation(groupLead->GetActorQuat());

    newCharacterInfo.AICharacter
        = world->SpawnActorDeferred<AACFCharacter>(
            ClassToSpawn, spawnTransform, nullptr, nullptr,
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    if (newCharacterInfo.AICharacter) {
        UGameplayStatics::FinishSpawningActor(newCharacterInfo.AICharacter, spawnTransform);

        // End Spawn
        if (!newCharacterInfo.AICharacter->GetController()) {
            newCharacterInfo.AICharacter->SpawnDefaultController();
        }
        uint8 localGroupIndex = AICharactersInfo.Num();
        newCharacterInfo.AIController = Cast<AACFAIController>(newCharacterInfo.AICharacter->GetController());
        if (newCharacterInfo.AIController) {
            InitAgent(newCharacterInfo, localGroupIndex);
        } else {
            UE_LOG(LogTemp, Error, TEXT("%s NO AI to Spawn - AAIFGROUPPAWN::SpawnAGroup"), *this->GetName());
        }

        AICharactersInfo.Add(newCharacterInfo);
        return localGroupIndex;
    }
    return 0;
}

uint8 UACFGroupAIComponent::AddExistingCharacterToGroup(AACFCharacter* character)
{
    UWorld* const world = GetWorld();

    if (!world) {
        return 0;
    }
        

	if (!groupLead) {
		SetReferences();
		if (!groupLead) {
			return -1;
		}
	}

    if (AICharactersInfo.Contains(character)) {
        return 0;
    }
       

    FAIAgentsInfo newCharacterInfo;
    newCharacterInfo.AICharacter = character;

    if (newCharacterInfo.AICharacter) {
        if (!newCharacterInfo.AICharacter->GetController()) {
            newCharacterInfo.AICharacter->SpawnDefaultController();
        }
          
        uint8 childIndex = AICharactersInfo.Num();
        newCharacterInfo.AIController = Cast<AACFAIController>(newCharacterInfo.AICharacter->GetController());
        if (newCharacterInfo.AIController) {
            InitAgent(newCharacterInfo, childIndex);
        } else {
            UE_LOG(LogTemp, Error, TEXT("%s NO AI to Spawn - AAIFGROUPPAWN::SpawnAGroup"), *this->GetName());
        }

        AICharactersInfo.Add(newCharacterInfo);
        return childIndex;
    }
    return 0;
}

AACFCharacter* UACFGroupAIComponent::GetAgentNearestTo(const FVector& location) const
{
    AACFCharacter* bestAgent = nullptr;
    float minDistance = 999999.f;
    for (FAIAgentsInfo achar : AICharactersInfo) {
        if (achar.AICharacter && achar.AICharacter->IsAlive()) {
            const float distance = FVector::Distance(location, achar.AICharacter->GetActorLocation());
            if (distance <= minDistance) {
                minDistance = distance;
                bestAgent = achar.AICharacter;
            }
        }
    }
    return bestAgent;
}

bool UACFGroupAIComponent::RemoveAgentFromGroup(AACFCharacter* character)
{
    if (!character)
        return false;

    AACFAIController* contr = Cast<AACFAIController>(character->GetController());
    if (!contr)
        return false;

    const FAIAgentsInfo agentInfo(character, contr);

    if (AICharactersInfo.Contains(agentInfo)) {
        AICharactersInfo.RemoveSingle(agentInfo);
        return true;
    }

    return false;
}

void UACFGroupAIComponent::SetInBattle(bool inBattle, AActor* newTarget)
{
    bInBattle = inBattle;
    if (bInBattle) {
        const APawn* aiTarget = Cast<APawn>(newTarget);
        if (aiTarget) {

            //Check if target is part of a group
            AController* targetCont = aiTarget->GetController();
            if (targetCont) {
                const bool bImplements = targetCont->GetClass()->ImplementsInterface(UACFGroupAgentInterface::StaticClass());
                if (bImplements && IACFGroupAgentInterface::Execute_IsPartOfGroup(targetCont)) {
                    UACFGroupAIComponent* groupComp = IACFGroupAgentInterface::Execute_GetOwningGroup(targetCont);
                    SetEnemyGroup(groupComp);
                } else {
                    enemyGroup = nullptr;
                }
            }
        }

        int32 index = 0;
        for (const FAIAgentsInfo& achar : AICharactersInfo) {
            if (!achar.AIController || achar.AIController->GetAIStateBK() == EAIState::EBattle || !achar.AICharacter->IsAlive()) {
                continue;
            }

            //Trying to assign to every agent in the group that is not in battle an enemy in the enemy group
            AActor* nextTarget = newTarget;
            FAIAgentsInfo adversary;
            if (enemyGroup && enemyGroup->GetGroupSize() > 0) {
                if (achar.AIController && !achar.AIController->HasTarget()) {
                    if (enemyGroup->GetGroupSize() > index) {
                        enemyGroup->GetAgentByIndex(index, adversary);
                        index++;
                    } else {
                        index = 0;
                        enemyGroup->GetAgentByIndex(index, adversary);
                        index++;
                    }
                    nextTarget = adversary.AICharacter;
                }
            }
            UACFThreatManagerComponent* threatComp = achar.AIController->GetThreatManager();
            if (nextTarget) {
                const float newThreat = threatComp->GetDefaultThreatForActor(nextTarget);
                if (newThreat > 0.f) {
                    //if the enemy we found is valid, we add aggro for that enemy
                    threatComp->AddThreat(nextTarget, newThreat + 10.f);
                } else {
                    //otherwise we go in the new target
                    threatComp->AddThreat(newTarget, threatComp->GetDefaultThreatForActor(newTarget));
                }
            } 
        }
    }
}


void UACFGroupAIComponent::OnChildDeath(const AACFCharacter* character)
{
    OnAgentDeath.Broadcast(character);
    const int32 index = AICharactersInfo.IndexOfByKey(character);
    if (AICharactersInfo.IsValidIndex(index)) {
        AICharactersInfo.RemoveAt(index);
    }

    if (AICharactersInfo.Num() == 0) {
        OnAllAgentDeath.Broadcast();
    }
}
