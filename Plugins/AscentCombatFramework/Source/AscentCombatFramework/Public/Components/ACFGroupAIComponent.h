// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once


#include "Actors/ACFCharacter.h"
#include "CoreMinimal.h"
#include "ACFAITypes.h"
#include <Components/SceneComponent.h>
#include <Components/SplineComponent.h>
#include <GameplayTagContainer.h>
#include "ACFGroupAIComponent.generated.h"

USTRUCT(BlueprintType)
struct FAISpawnInfo {
    GENERATED_BODY()

    FAISpawnInfo()
    {
        AIClassBP = AACFCharacter::StaticClass();
    };

    FAISpawnInfo(const TSubclassOf<class AACFCharacter> classToSpawn)
    {
        AIClassBP = classToSpawn;
    }

    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = ACF)
    TSubclassOf<AACFCharacter> AIClassBP;

    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadOnly, Category = ACF)
    /** the number of this ai class in group*/
    int32 TotalNumber = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    /** the max number of Col this ai class has in group*/
    int32 MaxNumOfCol = 5;

    UPROPERTY()
    int32 NumOfRow = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    /** the distance between each ai in same row */
    float ColumnPadding = 250;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
    /** the distance between each ai in same column */
    float RowPadding = 250;

    FORCEINLINE bool operator==(const FAISpawnInfo& Other) const
    {
        return this->AIClassBP == Other.AIClassBP;
    }

    FORCEINLINE bool operator!=(const FAISpawnInfo& Other) const
    {
        return this->AIClassBP != Other.AIClassBP;
    }

    FORCEINLINE bool operator==(const TSubclassOf<AACFCharacter>& Other) const
    {
        return this->AIClassBP == Other;
    }

    FORCEINLINE bool operator!=(const TSubclassOf<AACFCharacter>& Other) const
    {
        return this->AIClassBP != Other;
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAgentDeath, const AACFCharacter*, character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllAgentDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAgentsSpawned);

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class ASCENTCOMBATFRAMEWORK_API UACFGroupAIComponent : public USceneComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UACFGroupAIComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    virtual void SetReferences();

    UPROPERTY(BlueprintReadOnly, Category = ACF)
    bool bInBattle;

    /*Should use group or agent perception?*/
    UPROPERTY(EditAnywhere, Category = ACF)
    bool bOverrideAgentPerception = true;

    UPROPERTY(EditAnywhere, Category = ACF)
    bool bOverrideAgentTeam = true;

    UPROPERTY(EditAnywhere, Category = ACF)
    FVector2D spawnOffset;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = ACF)
    class AActor* groupLead;

    UPROPERTY(SaveGame, Replicated)
    TArray<FAIAgentsInfo> AICharactersInfo;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void OnComponentLoaded();

public:
    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnAgentDeath OnAgentDeath;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnAllAgentDeath OnAllAgentDeath;

    UPROPERTY(BlueprintAssignable, Category = ACF)
    FOnAgentsSpawned OnAgentsSpawned;

    UFUNCTION()
    virtual void OnChildDeath(const AACFCharacter* character);

    UFUNCTION(BlueprintCallable, Category = ACF)
    class AACFCharacter* GetAgentNearestTo(const FVector& location) const;

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE bool IsInBattle() const { return bInBattle; }

    UFUNCTION(BlueprintPure, Category = ACF)
    ETeam GetCombatTeam() const
    {
        return CombatTeam;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    FORCEINLINE int32 GetGroupSize() const { return AICharactersInfo.Num(); }

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool RemoveAgentFromGroup(AACFCharacter* character);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void GetGroupAgents(TArray<FAIAgentsInfo>& outAgents) const { outAgents = AICharactersInfo; }

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetInBattle(bool inBattle, AActor* newTarget);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void AddAIToSpawn(const TSubclassOf<AACFCharacter>& charClass);

    UFUNCTION(BlueprintCallable, Category = ACF)
    void RemoveAIToSpawn(const TSubclassOf<AACFCharacter>& charClass);

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool GetAgentByIndex(int32 index, FAIAgentsInfo& outAgent) const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    void SetPatrolPath(class AACFPatrolPath* inPatrol)
    {
        PatrolPath = inPatrol;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    class AACFPatrolPath* GetPatrolPath() const
    {
        return PatrolPath;
    }

    UFUNCTION(BlueprintPure, Category = ACF)
    class UACFGroupAIComponent* GetEnemyGroup() const { return enemyGroup; }

    UFUNCTION(BlueprintPure, Category = ACF)
    class AActor* GetGroupLead() const { return groupLead; }

    UFUNCTION(BlueprintPure, Category = ACF)
    FVector GetGroupCentroid() const;

    UFUNCTION(BlueprintCallable, Category = ACF)
    class AACFCharacter* RequestNewTarget(const AACFAIController* requestSender);

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
    void SendCommandToCompanions(FGameplayTag command);

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
    void SpawnGroup();

    UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
    void DespawnGroup(const bool bResetSpawnArray = false);

    void InitAgents();

    UFUNCTION(BlueprintCallable, Category = ACF)
    uint8 AddAgentToGroup(float inXOffset, float inYOffset, const TSubclassOf<AACFCharacter>& ClassToSpawn);

    UFUNCTION(BlueprintCallable, Category = ACF)
    uint8 AddExistingCharacterToGroup(AACFCharacter* character);

    UFUNCTION(BlueprintCallable, Category = ACF)
    bool IsAlreadyInGroup(const AACFCharacter* character) const
    {
        return AICharactersInfo.Contains(character);
    }

    UPROPERTY(EditAnywhere, SaveGame, BlueprintReadWrite, Category = ACF)
    TArray<FAISpawnInfo> AIToSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    ETeam CombatTeam = ETeam::ETeam1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    EAIState DefaultAiState = EAIState::EPatrol;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    bool CanSpawnMultitpleTimes = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
    uint8 MaxSimultaneousAgents = 20;

private:
    UPROPERTY(SaveGame)
    bool bAlreadySpawned = false;

    void Internal_SendCommandToAgents(FGameplayTag command);

    UPROPERTY()
    UACFGroupAIComponent* enemyGroup;

    UPROPERTY()
    TArray<int32> FormationInfo;

    UPROPERTY()
    AACFPatrolPath* PatrolPath;

    void Internal_SpawnGroup();

    void InitAgent(FAIAgentsInfo& agent, int32 childIndex);

    void SetEnemyGroup(UACFGroupAIComponent* inEnemyGroup);
};
