// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACFCoreTypes.h"
#include "ACFTeamManagerComponent.generated.h"

const int32 MaxTeam = 5;

UCLASS( ClassGroup=(ACF), meta=(BlueprintSpawnableComponent) )
class ASCENTCOREINTERFACES_API UACFTeamManagerComponent : public UActorComponent
{
	GENERATED_BODY()

// Constructors
public:	
	// Sets default values for this component's properties
	UACFTeamManagerComponent();
// Methods
public:	
	
	TArray<TEnumAsByte<ECollisionChannel>> GetAllCollisionChannels(const bool bIgnoreNeutral) const;
	TArray<TEnumAsByte<ECollisionChannel>> GetEnemiesCollisionChannels(const ETeam SelfTeam) const;

	UFUNCTION(BlueprintPure, Category = ACF)
	TEnumAsByte<ECollisionChannel> GetCollisionChannelByTeam(ETeam Team) const;
	
	UFUNCTION(BlueprintPure, Category = ACF)
	bool IsEnemyTeam(ETeam SelfTeam, ETeam TargetTeam) const;

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE EBattleType GetBattleType() const { return BattleType; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, Category = ACF)
	EBattleType BattleType = EBattleType::ETeamBased;
// Members
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize, Category = ACF)
	TMap<ETeam, FTeamInfo> Teams;

private:

	TArray<TEnumAsByte<ECollisionChannel>> Internal_GetEnemiesCollisionChannels(const ETeam SelfTeam) const;

};