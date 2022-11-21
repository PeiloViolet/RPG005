// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "Components/ACFDamageHandlerComponent.h"
#include <Engine/EngineTypes.h>
#include <GameFramework/Controller.h>
#include <GameFramework/Actor.h>
#include <GameFramework/DamageType.h>
#include "Game/ACFFunctionLibrary.h"
#include <PhysicsEngine/BodyInstance.h>
#include "ARSStatisticsComponent.h"
#include "Actors/ACFCharacter.h"
#include "Game/ACFDamageType.h"
#include "ARSTypes.h"
#include "Components/ACFTeamManagerComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Components/MeshComponent.h>
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UACFDamageHandlerComponent::UACFDamageHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bIsAlive=true;
}



void UACFDamageHandlerComponent::InitializeDamageCollisions(ETeam combatTeam)
{
	UACFTeamManagerComponent* TeamManager = UACFFunctionLibrary::GetACFTeamManager(GetWorld());
	if (TeamManager) {
		AssignCollisionProfile(TeamManager->GetCollisionChannelByTeam(combatTeam));
		inCombatTeam = combatTeam;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Remember to add an ACFGameState to your World!!! -  UACFDamageHandlerComponent::InitializeDamageCollisions"));
	}
}

void UACFDamageHandlerComponent::AssignCollisionProfile(const TEnumAsByte<ECollisionChannel> channel)
{
	TArray<UActorComponent*> meshes;
	GetOwner()->GetComponents(UMeshComponent::StaticClass(), meshes);

	for (auto& mesh : meshes) {
		UMeshComponent* meshComp = Cast< UMeshComponent>(mesh);

		if (meshComp) {
			meshComp->SetCollisionObjectType(channel);
		}
	}
}

float UACFDamageHandlerComponent::TakeDamage(class AActor* damageReceiver, float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!damageReceiver)
		return 0;

	FHitResult outDamage;
	FVector ShotDirection;
	DamageEvent.GetBestHitInfo(damageReceiver, DamageCauser, outDamage, ShotDirection);

	ConstructDamageReceived(damageReceiver, Damage, EventInstigator, outDamage.Location,
		outDamage.Component.Get(), outDamage.BoneName,
		ShotDirection, DamageEvent.DamageTypeClass,
		DamageCauser);

	if (!DamageCalculator ) {
		DamageCalculator =
			NewObject<UACFDamageCalculation>(this, UACFFunctionLibrary::GetDamageCalculationClass());
	}

	if (DamageCalculator) {
		FACFDamageEvent outDamageEvent;
		// APPARENTLY USELESS STUFF TO LET YOU MODIFY IN BLUEPRINT THE DAMAGE WITH
		// YOUR OWN CALCULATION
		LastDamageReceived = DamageCalculator->CalculateFinalDamage(LastDamageReceived);
		
		UARSStatisticsComponent* StatisticsComp = damageReceiver->FindComponentByClass<UARSStatisticsComponent>();

		if (StatisticsComp) {
			FStatisticModifier statMod(UACFFunctionLibrary::GetHealthTag(), -LastDamageReceived.FinalDamage);
			StatisticsComp->ModifyStat(statMod);
		}
		ClientsReceiveDamage(LastDamageReceived);
		return LastDamageReceived.FinalDamage;
	}
	return 0.f;
}

void UACFDamageHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	UARSStatisticsComponent* StatisticsComp = GetOwner()->FindComponentByClass< UARSStatisticsComponent>();
	if (StatisticsComp) {

		StatisticsComp->OnStatisiticReachesZero.AddDynamic(
			this, &UACFDamageHandlerComponent::HandleStatReachedZero);
	}
}

void UACFDamageHandlerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UACFDamageHandlerComponent, bIsAlive);

}

void UACFDamageHandlerComponent::ConstructDamageReceived(
	AActor* DamagedActor, float Damage, class AController* InstigatedBy,
	FVector HitLocation, class UPrimitiveComponent* FHitComponent,
	FName BoneName, FVector ShotFromDirection,
	TSubclassOf<UDamageType> DamageType, AActor* DamageCauser) {

	LastDamageReceived.contextString = NAME_None;
	LastDamageReceived.FinalDamage = Damage;
	LastDamageReceived.DamageDealer = Cast<APawn>(DamageCauser);
	LastDamageReceived.hitDirection = ShotFromDirection;
	LastDamageReceived.hitResult.BoneName = BoneName;
	LastDamageReceived.hitResult.ImpactPoint = HitLocation;
	LastDamageReceived.hitResult.Location = HitLocation;
	LastDamageReceived.hitResult.HitObjectHandle = FActorInstanceHandle(DamagedActor);
	LastDamageReceived.DamageReceiver = DamagedActor;
	LastDamageReceived.DamageClass = DamageType;

	LastDamageReceived.DamageDirection =
		UACFFunctionLibrary::GetHitDirectionByHitResult(
			LastDamageReceived.DamageDealer, LastDamageReceived.hitResult);

	LastDamageReceived.HitResponseAction = FGameplayTag();
	
	AACFCharacter* acfReceiver = Cast<AACFCharacter>(DamagedActor);

	if (acfReceiver) {
		LastDamageReceived.DamageZone = acfReceiver->GetDamageZoneByBoneName(BoneName);
		FBodyInstance* bodyInstance = acfReceiver->GetMesh()->GetBodyInstance(BoneName);
		if (bodyInstance) {
			LastDamageReceived.PhysMaterial = bodyInstance->GetSimplePhysicalMaterial();
		}
		LastDamageReceived.HitResponseAction = EvaluateHitResponse(LastDamageReceived);
		
	}
	
}

void UACFDamageHandlerComponent::HandleStatReachedZero(FGameplayTag stat)
{
	if (UACFFunctionLibrary::GetHealthTag() == stat) {

		if (GetOwner()->HasAuthority()) {
			UARSStatisticsComponent* StatisticsComp = GetOwner()->FindComponentByClass<UARSStatisticsComponent>();
			if (StatisticsComp) {
				StatisticsComp->StopRegeneration();
				if (LastDamageReceived.DamageDealer) {
					UARSStatisticsComponent* dealerStatComp = LastDamageReceived.DamageDealer->FindComponentByClass<UARSStatisticsComponent>();
					if (dealerStatComp) {
						dealerStatComp->AddExp(StatisticsComp->GetExpOnDeath());
					}
				}
			}
		}
		bIsAlive = false;
		OnOwnerDeath.Broadcast();
	}
}


void UACFDamageHandlerComponent::ClientsReceiveDamage_Implementation(
	const FACFDamageEvent& damageEvent) {
	LastDamageReceived = damageEvent;

	OnDamageReceived.Broadcast(damageEvent);
}

bool UACFDamageHandlerComponent::ClientsReceiveDamage_Validate(const FACFDamageEvent& damageEvent) {
	return true;
}


FGameplayTag UACFDamageHandlerComponent::EvaluateHitResponse(const FACFDamageEvent& damageEvent)
{
	const AACFCharacter* charOwn = Cast< AACFCharacter>(GetOwner());
	if (charOwn) {
		for (auto& elem : HitResponseActions)
		{
			if (UACFFunctionLibrary::ShouldExecuteAction(elem, charOwn) && EvaluetHitResponseAction(elem, damageEvent))
			{
				return elem.ActionTag;
			}
		}
	}


	return FGameplayTag();
}

bool UACFDamageHandlerComponent::EvaluetHitResponseAction(const FOnHitActionChances& action, const FACFDamageEvent& damageEvent)
{
	if ((uint8)damageEvent.DamageDirection != (uint8)action.AllowedFromDirection &&
		action.AllowedFromDirection != EActionDirection::EveryDirection)
		return false;

	for (const TSubclassOf<UDamageType>& damageType : action.AllowedDamageTypes)
	{
		if (damageEvent.DamageClass->IsChildOf(damageType))
			return true;
	}
	return false;
}

