// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <GameplayTagContainer.h>
#include "Game/ACFTypes.h"
#include <GameFramework/Pawn.h>
#include <GenericTeamAgentInterface.h>
#include "ACFPatrolPath.h"
#include "ACFAIGroupSpawner.generated.h"
  


UCLASS()
class ASCENTCOMBATFRAMEWORK_API AACFAIGroupSpawner : public APawn, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:	

	AACFAIGroupSpawner();


// 	friend class AACFAIController;
// 	friend class UACFGroupAIComponent;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class USplineComponent* SplineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UACFGroupAIComponent* AIGroupComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	bool bSpawnOnBeginPlay = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	FPatrolConfig PatrolConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ACF)
	float UpdateGroupPositionTimeInterval = 1.f;

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

public:

	UFUNCTION(BlueprintCallable, Category = ACF)
	FVector GetPatrolPoint(int32 patrolIndex) const;

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class USplineComponent* GetSplineComponent() const { return patrolPath->GetSplineComponent(); }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class UACFGroupAIComponent* GetAIGroupComponent() const { return AIGroupComponent; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE  ETeam GetCombatTeam() const { return CombatTeam; }

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool GetAgentWithIndex(int32 index, FAIAgentsInfo& outAgent) const;
	
	UFUNCTION(BlueprintCallable, Category = ACF)
	AACFCharacter* GetAgentNearestTo(const FVector& location) const;

private:
	UPROPERTY()
	ETeam CombatTeam = ETeam::ETeam1;

	UPROPERTY()
	AACFPatrolPath* patrolPath;

	UFUNCTION()
	void StartUpdatePos();

	UFUNCTION()
	void StopUpdatePos();

	UFUNCTION()
	void AdjustSpawnerPos();

	bool bCheckPos = false;

	FTimerHandle timerHandle;
};
