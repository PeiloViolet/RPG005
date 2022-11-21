// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "ACFItemSystemFunctionLibrary.h"
#include "Items/ACFWorldItem.h"
#include "Items/ACFItem.h"
#include "Items/ACFEquippableItem.h"
#include "UObject/SoftObjectPtr.h"
#include "Items/ACFConsumable.h"
#include "ACFInventorySettings.h"
#include "GameplayTagsManager.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"



AACFWorldItem* UACFItemSystemFunctionLibrary::SpawnWorldItemNearLocation(UObject* WorldContextObject, const FBaseItem& ContainedItem, const FVector& location, float acceptanceRadius /*= 100.f*/)
{
	FVector outLoc;

	if (ContainedItem.Count <= 0) {
		return nullptr;
	}
	if (UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(WorldContextObject, location, outLoc, acceptanceRadius)) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		TSubclassOf<AACFWorldItem> WorldItemClass = UACFItemSystemFunctionLibrary::GetDefaultWorldItemClass();
		ensure(WorldItemClass);

		APlayerController* playerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
		UWorld* world = playerController->GetWorld();

		if (!world) {
			return nullptr;
		}

		if (world && WorldItemClass) {
			AACFWorldItem* _worldItem = world->SpawnActor<AACFWorldItem>(WorldItemClass, outLoc, FRotator(0), SpawnParams);
			if (_worldItem) {
				_worldItem->AddItem(ContainedItem);
				return _worldItem;
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Impossible to Spawn Item!! MISSING NAVMESH - UACFFunctionLibrary::SpawnWorldItemNearLocation"));
		}
	}
	return nullptr;
}


bool UACFItemSystemFunctionLibrary::GetItemData(const TSubclassOf<class AACFItem>& item, FItemDescriptor& outData)
{
/*	item.LoadSynchronous();*/
	if (item) {
		const AACFItem* itemInstance = Cast<AACFItem>(item.Get()->GetDefaultObject());
		if (itemInstance) {
			outData = itemInstance->GetItemInfo();
			return true;
		}
	}
	return false;
}

bool UACFItemSystemFunctionLibrary::GetEquippableAttributeSetModifier(const TSubclassOf<class AACFItem>& itemClass, FAttributesSetModifier& outModifier)
{
/*	itemClass.LoadSynchronous();*/
	if (itemClass) {
		const AACFEquippableItem* itemInstance = Cast<AACFEquippableItem>(itemClass.GetDefaultObject());
		if (itemInstance) {
			outModifier = itemInstance->GetAttributeSetModifier();
			return true;
		}
	}
	return false;
}

bool UACFItemSystemFunctionLibrary::GetEquippableAttributeRequirements(const TSubclassOf<class AACFItem>& itemClass, TArray<FAttribute>& outAttributes)
{
/*	itemClass.LoadSynchronous();*/
	if (itemClass) {
		const AACFEquippableItem* itemInstance = Cast<AACFEquippableItem>(itemClass.GetDefaultObject());
		if (itemInstance) {
			outAttributes = itemInstance->GetAttributeRequirement();
			return true;
		}
	}
	return false;
}

bool UACFItemSystemFunctionLibrary::GetConsumableTimedAttributeSetModifier(const TSubclassOf<class AACFItem>& itemClass, TArray<FTimedAttributeSetModifier>& outModifiers)
{
/*	itemClass.LoadSynchronous();*/
	if (itemClass) {
		const AACFConsumable* itemInstance = Cast<AACFConsumable>(itemClass.GetDefaultObject());
		if (itemInstance) {
			outModifiers = itemInstance->GetTimedModifiers();
			return true;
		}
	}
	return false;
}

bool UACFItemSystemFunctionLibrary::GetConsumableStatModifier(const TSubclassOf<class AACFItem>& itemClass, TArray <FStatisticModifier>& outModifiers)
{
	/*itemClass.LoadSynchronous();*/
	if (itemClass) {
		const AACFConsumable* itemInstance = Cast<AACFConsumable>(itemClass.GetDefaultObject());
		if (itemInstance) {
			outModifiers = itemInstance->GetStatsModifiers();
			return true;
		}
	}
	return false;
}


FGameplayTag UACFItemSystemFunctionLibrary::GetItemTypeTagRoot()
{
	UACFInventorySettings* settings = GetMutableDefault<UACFInventorySettings>();

	if (settings) {
		return settings->ItemtypeTag;
	}
	UE_LOG(LogTemp, Warning, TEXT("Missing Tag! - UACFItemSystemFunctionLibrary "));

	return FGameplayTag();
}

FGameplayTag UACFItemSystemFunctionLibrary::GetItemSlotTagRoot()
{
	UACFInventorySettings* settings = GetMutableDefault<UACFInventorySettings>();

	if (settings) {
		return settings->ItemSlotsTag;
	}
	UE_LOG(LogTemp, Warning, TEXT("Missing Tag! - UACFItemSystemFunctionLibrary "));

	return FGameplayTag();
}

TSubclassOf<AACFWorldItem> UACFItemSystemFunctionLibrary::GetDefaultWorldItemClass()
{
	UACFInventorySettings* settings = GetMutableDefault<UACFInventorySettings>();

	if (settings) {
		return settings->WorldItemClass;
	}
	UE_LOG(LogTemp, Warning, TEXT("Missing Default Item Class! - UACFFunctionLibrary "));

	return nullptr;
}

bool UACFItemSystemFunctionLibrary::IsValidItemTypeTag(FGameplayTag TagToCheck)
{
	const FGameplayTag root = GetItemTypeTagRoot();
	return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}

bool UACFItemSystemFunctionLibrary::IsValidItemSlotTag(FGameplayTag TagToCheck)
{
	const FGameplayTag root = GetItemSlotTagRoot();
	return UGameplayTagsManager::Get().RequestGameplayTagChildren(root).HasTag(TagToCheck);
}