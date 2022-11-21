// Copyright (C) Fred Chan. 2020. All Rights Reserved.


#include "Components/ACFTeamManagerComponent.h"
#include "Engine/CollisionProfile.h"

// Sets default values for this component's properties
UACFTeamManagerComponent::UACFTeamManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;   

	// Default teams
	for(uint8 i = 0; i < MaxTeam;i++)
	{
		FTeamInfo TeamInfo = FTeamInfo();

		ETeam CurrentTeam = static_cast<ETeam>(i);
		TeamInfo.DisplayName = FText::FromString(FString::Printf(TEXT("Team %d"),(i+1)));


		for(uint8 j = 0; j < MaxTeam; j++)
		{
			
			const ETeam TargetTeam = static_cast<ETeam>(j);
			ETeamAttitude::Type Attitude;

			if(CurrentTeam == ETeam::ENeutral)
			{
				Attitude = ETeamAttitude::Neutral;
			}else
			{
				if(CurrentTeam == TargetTeam)
				{
					Attitude = ETeamAttitude::Friendly;
				}
				else if(TargetTeam == ETeam::ENeutral)
				{
					Attitude = ETeamAttitude::Neutral;
				}else
				{
					Attitude = ETeamAttitude::Hostile;
				}
			}

			TeamInfo.Relationship.Add(TargetTeam, Attitude);
			
		}
		
		Teams.Add(CurrentTeam, TeamInfo);
	}
}


// Called when the game starts
void UACFTeamManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

TArray<TEnumAsByte<ECollisionChannel>> UACFTeamManagerComponent::Internal_GetEnemiesCollisionChannels(const ETeam SelfTeam) const
{
	TArray<TEnumAsByte<ECollisionChannel>> channels;

	for (uint8 i = 0; i < MaxTeam; i++)
	{
		const ETeam TargetTeam = static_cast<ETeam>(i);

		if (IsEnemyTeam(SelfTeam, TargetTeam))
		{
			channels.Add(GetCollisionChannelByTeam(TargetTeam));
		}
	}
	return channels;
}

TArray<TEnumAsByte<ECollisionChannel>> UACFTeamManagerComponent::GetAllCollisionChannels(bool bIgnoreNeutral) const
{
	TArray<TEnumAsByte<ECollisionChannel>> channels;
	
	for(int32 i = 0; i < MaxTeam;i++)
	{
		const ETeam TargetTeam = static_cast<ETeam>(i);
		if (TargetTeam == ETeam::ENeutral && bIgnoreNeutral) {
			continue;
		}		
		channels.Add(GetCollisionChannelByTeam(TargetTeam));
	}
	return channels;
}

TArray<TEnumAsByte<ECollisionChannel>> UACFTeamManagerComponent::GetEnemiesCollisionChannels(const ETeam SelfTeam) const
{
	if (BattleType == EBattleType::EEveryoneAgainstEveryone) {
		return GetAllCollisionChannels(true);
	}
	else {
		return Internal_GetEnemiesCollisionChannels(SelfTeam);
	}
	return TArray<TEnumAsByte<ECollisionChannel>>();
}

TEnumAsByte<ECollisionChannel> UACFTeamManagerComponent::GetCollisionChannelByTeam(ETeam Team) const 
{
	if (Teams.Contains(Team)) {
		return Teams.Find(Team)->CollisionChannel;
	}
	
	UE_LOG(LogTemp, Error, TEXT("INVALID TEAM! - UACFTeamManagerComponent "));
	return ECollisionChannel::ECC_MAX;
}


bool UACFTeamManagerComponent::IsEnemyTeam(ETeam SelfTeam, ETeam TargetTeam) const
{
	if (BattleType ==
      EBattleType::EEveryoneAgainstEveryone)
	{
		return true;
	}

	if (Teams.Contains(SelfTeam)) {
		const FTeamInfo* teamInfo = Teams.Find(SelfTeam);
		if (teamInfo && teamInfo->Relationship.Contains(TargetTeam)) {
			return teamInfo->Relationship.Find(TargetTeam)->GetValue() == ETeamAttitude::Hostile;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("INVALID TEAM RELATIONSHIPS CONFIG! CHECK GAME STATE- UACFTeamManagerComponent "));

	return false;
}

