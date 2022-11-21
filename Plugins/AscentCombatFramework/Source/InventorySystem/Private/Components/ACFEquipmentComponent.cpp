// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "Components/ACFEquipmentComponent.h"
#include "ARSStatisticsComponent.h"
#include "Items/ACFAccessory.h"
#include "Items/ACFArmor.h"
#include "Items/ACFConsumable.h"
#include "Items/ACFEquippableItem.h"
#include "Items/ACFItem.h"
#include "ACFItemSystemFunctionLibrary.h"
#include "Items/ACFMeleeWeapon.h"
#include "Items/ACFRangedWeapon.h"
#include "Items/ACFWeapon.h"
#include "Items/ACFWorldItem.h"
#include "Net/UnrealNetwork.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <GameplayTagContainer.h>
#include <Kismet/KismetSystemLibrary.h>
#include <NavigationSystem.h>
#include "Items/ACFProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ACFStorageComponent.h"
#include "ACFItemSystemFunctionLibrary.h"
#include "GameFramework/Character.h"


void UACFEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UACFEquipmentComponent, Equipment);
    DOREPLIFETIME(UACFEquipmentComponent, Inventory);
    DOREPLIFETIME(UACFEquipmentComponent, currentInventoryWeight);
    DOREPLIFETIME(UACFEquipmentComponent, CurrentlyEquippedSlotType);
}

// Sets default values for this component's properties
UACFEquipmentComponent::UACFEquipmentComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
    Inventory.Empty();
}


void UACFEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();
	CharacterOwner = Cast<ACharacter>(GetOwner());
	if (CharacterOwner) {
		MainCharacterMesh = CharacterOwner->GetMesh();
	}
}



void UACFEquipmentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Internal_DestroyEquipment();
    Super::EndPlay(EndPlayReason);
}

void UACFEquipmentComponent::OnComponentLoaded_Implementation()
{   
    DestroyEquipment();

    Equipment.EquippedItems.Empty();
    for (auto& slot : Inventory) {
        UACFItemSystemFunctionLibrary::GetItemData(slot.itemClass, slot.ItemInfo);
        if (slot.bIsEquipped) {
            EquipItemFromInventory(slot);
        }
    }
}

void UACFEquipmentComponent::AddItemToInventory_Implementation(const FBaseItem& ItemToAdd)
{
    Internal_AddItem(ItemToAdd);
}

bool UACFEquipmentComponent::AddItemToInventory_Validate(const FBaseItem& ItemToAdd)
{
    return true;
}

void UACFEquipmentComponent::AddItemToInventoryByClass_Implementation(TSubclassOf<AACFItem> inItem, int32 count /*= 1*/)
{
    AddItemToInventory(FBaseItem(inItem, count));
}

bool UACFEquipmentComponent::AddItemToInventoryByClass_Validate(TSubclassOf<AACFItem> ItemToAdd, int32 count /*= 1*/)
{
    return true;
}

void UACFEquipmentComponent::RemoveItemByIndex_Implementation(const int32 index, int32 count /*= 1*/)
{
    if (Inventory.IsValidIndex(index)) {
        RemoveItem(Inventory[index], count);
    }
}

bool UACFEquipmentComponent::RemoveItemByIndex_Validate(const int32 index, int32 count /*= 1*/)
{
    return true;
}

void UACFEquipmentComponent::DropItem_Implementation(const FInventoryItem& item, int32 count /*= 1*/)
{
    FInventoryItem* itemptr = Internal_GetInventoryItem(item);
    if (!itemptr) {
        return;
    }

    SpawnWorldItem(count, item.itemClass);

    RemoveItem(item, count);
}

bool UACFEquipmentComponent::DropItem_Validate(const FInventoryItem& item, int32 count /*= 1*/)
{
    return true;
}

void UACFEquipmentComponent::RemoveItem_Implementation(const FInventoryItem& item, int32 count /*= 1*/)
{
    FInventoryItem* itemptr = Internal_GetInventoryItem(item);
    if (itemptr) {
        const float weightRemoved = count * itemptr->ItemInfo.ItemWeight;
        itemptr->Count -= count;

        if (itemptr->bIsEquipped && Equipment.EquippedItems.Contains(item.ItemInfo.ItemSlot)) {
            const int32 index = Equipment.EquippedItems.IndexOfByKey(item.ItemInfo.ItemSlot);
            Equipment.EquippedItems[index].InventoryItem.Count = itemptr->Count;
            OnEquipmentChanged.Broadcast(Equipment);
        }
        if (itemptr->Count <= 0) {      
            if (itemptr->bIsEquipped) {
                FEquippedItem outItem;
                GetEquippedItemSlot(itemptr->ItemInfo.ItemSlot, outItem);
                UnequipItemBySlot(itemptr->ItemInfo.ItemSlot);
            }     
            FInventoryItem toBeRemoved;
            if (GetItemByGuid(itemptr->GetItemGuid(), toBeRemoved)) {
                Inventory.Remove(toBeRemoved);
            }
        }
        currentInventoryWeight -= weightRemoved;
        OnInventoryChanged.Broadcast(Inventory);
    }
}

bool UACFEquipmentComponent::RemoveItem_Validate(const FInventoryItem& item, int32 count /*= 1*/)
{
    return true;
}

void UACFEquipmentComponent::UseInventoryItem_Implementation(const FInventoryItem& item)
{
    FInventoryItem invItem;
    if (GetItemByGuid(item.GetItemGuid(), invItem)) {
		if (!invItem.bIsEquipped) {
			EquipItemFromInventory(invItem);
		}
		else {
			UnequipItemBySlot(invItem.ItemInfo.ItemSlot);
		}
    }
}

void UACFEquipmentComponent::UseInventoryItemByIndex_Implementation(int32 index)
{
	if (Inventory.IsValidIndex(index)) {
		FInventoryItem item = Inventory[index];
        UseInventoryItem(item);
	}
}

bool UACFEquipmentComponent::UseInventoryItemByIndex_Validate(int32 index)
{
	return true;
}

bool UACFEquipmentComponent::UseInventoryItem_Validate(const FInventoryItem& item)
{
    return true;
}

bool UACFEquipmentComponent::HasEnoughItemsOfType(const TArray<FBaseItem>& ItemsToCheck)
{
    for (const auto& item : ItemsToCheck) {

        int32 numberToCheck = item.Count;
        TArray<FInventoryItem*> invItems = FindItemsByClass(item.ItemClass);
        int32 TotItems = 0;
        for (const auto& invItem : invItems) {
            if (invItem) {
                TotItems += invItem->Count;
            }
        }

        if (TotItems < numberToCheck)
            return false;
    }
    return true;
}

FGameplayTag UACFEquipmentComponent::GetCurrentDesiredMovesetTag() const
{
	if (Equipment.SecondaryWeapon && Equipment.SecondaryWeapon->OverridesMainHandMoveset()) {
		return Equipment.SecondaryWeapon->GetAssociatedMovesetTag();
	}
	else if (Equipment.MainWeapon){
		return Equipment.MainWeapon->GetAssociatedMovesetTag();
	}
    return FGameplayTag();
}

FGameplayTag UACFEquipmentComponent::GetCurrentDesiredMovesetActionTag() const
{
	if (Equipment.SecondaryWeapon && Equipment.SecondaryWeapon->OverridesMainHandMovesetActions()) {
		return Equipment.SecondaryWeapon->GetAssociatedMovesetActionsTag();
	}
	else if (Equipment.MainWeapon) {
		return Equipment.MainWeapon->GetAssociatedMovesetActionsTag();
	}
	return FGameplayTag();
}

FGameplayTag UACFEquipmentComponent::GetCurrentDesiredOverlayTag() const
{
	if (Equipment.SecondaryWeapon && Equipment.SecondaryWeapon->OverridesMainHandOverlay()) {
		return Equipment.SecondaryWeapon->GetAssociatedMovesetOverlayTag();
	}
	else if (Equipment.MainWeapon) {
		return Equipment.MainWeapon->GetAssociatedMovesetOverlayTag();
	}
	return FGameplayTag();
}


void UACFEquipmentComponent::ConsumeItems_Implementation(const TArray<FBaseItem>& ItemsToCheck)
{
    for (const auto& item : ItemsToCheck) {
        const TArray<FInventoryItem*> invItems = FindItemsByClass(item.ItemClass);
        if (invItems.IsValidIndex(0)) {
            RemoveItem(*(invItems[0]), item.Count);
        }
    }
}

bool UACFEquipmentComponent::ConsumeItems_Validate(const TArray<FBaseItem>& ItemsToCheck)
{
    return true;
}

void UACFEquipmentComponent::MoveItemsToInventory_Implementation(const TArray<FBaseItem>& inItems, UACFStorageComponent* storage)
{
    if (!storage) {
		UE_LOG(LogTemp, Error, TEXT("Invalid Storage, verify that the owner of this component is repliacted! - ACFEquipmentComp"));
        return;
    }

	TArray<FBaseItem> pendingRemove;
	for (const auto& item : inItems) {
		const int32 numItems = UKismetMathLibrary::Min(NumberOfItemCanTake(item.ItemClass), item.Count);
		AddItemToInventoryByClass(item.ItemClass, numItems);
		pendingRemove.Add(FBaseItem(item.ItemClass, numItems));
	}
    storage->RemoveItems(pendingRemove);
}

bool UACFEquipmentComponent::MoveItemsToInventory_Validate(const TArray<FBaseItem>& inItems, UACFStorageComponent* equipComp)
{
    return true;
}

void UACFEquipmentComponent::OnRep_Equipment()
{
    OnEquipmentChanged.Broadcast(Equipment);
}

void UACFEquipmentComponent::OnRep_Inventory()
{
    OnInventoryChanged.Broadcast(Inventory);
}

void UACFEquipmentComponent::OnEntityOwnerDeath_Implementation()
{
	if (CharacterOwner) {
		TArray<AActor*> attachedActors;
		CharacterOwner->GetAttachedActors(attachedActors, true);
        TArray<FBaseItem> projCount;
		for (const auto actor : attachedActors) {
			AACFProjectile* proj = Cast<AACFProjectile>(actor);
			if (proj) {
                const float percentage = FMath::RandRange(0.f, 100.f);
				if (proj->ShouldBeDroppedOnDeath()  && proj->GetDropOnDeathPercentage() >= percentage) {     
                    if (projCount.Contains(proj->GetClass())) {
                        FBaseItem* actualItem = projCount.FindByKey(proj->GetClass());
                        actualItem->Count +=  1;    
                    }
                    else {
						projCount.Add(FBaseItem(proj->GetClass(), 1));
                    }
				}
				proj->SetLifeSpan(0.2f);
			}
		}
        for (const auto& item : projCount) {
            UACFItemSystemFunctionLibrary::SpawnWorldItemNearLocation(this, item, CharacterOwner->GetActorLocation(), 50.f);
        }
	}
    DestroyEquippedItems();

}

void UACFEquipmentComponent::Internal_OnArmorUnequipped_Implementation(const FGameplayTag& slot)
{
    FModularPart outMesh;
    FEquippedItem outEquip;

	if (GetModularMesh(slot, outMesh) && outMesh.meshComp) {
		outMesh.meshComp->SetSkeletalMesh(nullptr);
	}
}

bool UACFEquipmentComponent::Internal_OnArmorUnequipped_Validate(const FGameplayTag& slot)
{
    return true;
}

bool UACFEquipmentComponent::CanBeEquipped(const TSubclassOf<AACFItem>& equippable)
{
    FItemDescriptor ItemData;
    TArray<FAttribute> attributes;
    UACFItemSystemFunctionLibrary::GetItemData(equippable, ItemData);

    if (!AvailableEquipmentSlot.Contains(ItemData.ItemSlot)) {
        return false;
    }
    if (UACFItemSystemFunctionLibrary::GetEquippableAttributeRequirements(equippable, attributes)) {
        const UARSStatisticsComponent* _statcomp = CharacterOwner->FindComponentByClass<UARSStatisticsComponent>();
        if (_statcomp) {
            return _statcomp->CheckAttributesRequirements(attributes);
        } else {
            UE_LOG(LogTemp, Log, TEXT("Add UARSStatisticsComponent to your character!! - ACFEquipmentComp"));
        }
    } else {
        return true;
    }
    return false;
}

int32 UACFEquipmentComponent::Internal_AddItem(const FBaseItem& itemToAdd, bool bTryToEquip /*= true*/, float dropChancePercentage /*= 0.f*/)
{
    int32 addeditemstotal = 0;
    int32 addeditemstmp = 0;
    bool bSuccessful = false;
    FItemDescriptor itemData;
    UACFItemSystemFunctionLibrary::GetItemData(itemToAdd.ItemClass, itemData);
    const int32 itemweight = itemData.ItemWeight;
    const int32 maxAddableByWeightTotal = FMath::TruncToInt((MaxInventoryWeight - currentInventoryWeight) / itemData.ItemWeight);
    int32 count = itemToAdd.Count;
    if (maxAddableByWeightTotal < itemToAdd.Count) {
        count = maxAddableByWeightTotal;
    }
    int MaxInventoryStack = itemData.MaxInventoryStack;
    TArray<FInventoryItem*> outItems = FindItemsByClass(itemToAdd.ItemClass);
    //IF WE ALREADY HAVE SOME ITEMS LIKE THAT, INCREMENT ACTUAL VALUE

    bool bGate = true;
    if (outItems.Num() > 0) {
        for (const auto& outItem : outItems) {
            if (outItem->Count < itemData.MaxInventoryStack) {
                if (outItem->Count + count <= itemData.MaxInventoryStack && count * itemData.ItemWeight + currentInventoryWeight <= MaxInventoryWeight) {
                    addeditemstmp = count;
                } else {
                    int32 maxAddableByStack = itemData.MaxInventoryStack - outItem->Count;
                    addeditemstmp = maxAddableByStack;
                }

                outItem->Count += addeditemstmp;
                addeditemstotal += addeditemstmp;
                count -= addeditemstmp;
                outItem->DropChancePercentage = dropChancePercentage;
                if (outItem->bIsEquipped && Equipment.EquippedItems.Contains(itemData.ItemSlot)) {
                    int32 index = Equipment.EquippedItems.IndexOfByKey(itemData.ItemSlot);
                    Equipment.EquippedItems[index].InventoryItem.Count = outItem->Count;
                    OnEquipmentChanged.Broadcast(Equipment);
                } else if (bTryToEquip && !Equipment.EquippedItems.Contains(outItem->ItemInfo.ItemSlot)) {
                    EquipItemFromInventory(*outItem);
                }
                bSuccessful = true;
                //break;
            }
        }
    }

    //Otherwise we add new
    const int32 NumberOfItemNeed = FMath::CeilToInt((float)count / (float)itemData.MaxInventoryStack);
    const int32 FreeSpaceInInventory = MaxInventorySlots - Inventory.Num();
    const int32 NumberOfStackToCreate = FGenericPlatformMath::Min(NumberOfItemNeed, FreeSpaceInInventory);
    for (int i = 0; i < NumberOfStackToCreate; i++) {
        if (Inventory.Num() < MaxInventorySlots) {
            FInventoryItem newItem(itemToAdd);
            if (count > itemData.MaxInventoryStack) {
                newItem.Count = itemData.MaxInventoryStack;
            } else {
                newItem.Count = count;
            }
            newItem.DropChancePercentage = dropChancePercentage;

            newItem.InventoryIndex = GetFirstEmptyInventoryIndex();
            addeditemstotal += newItem.Count;
            count -= newItem.Count;
            Inventory.Add(newItem);
            if (bTryToEquip && !Equipment.EquippedItems.Contains(newItem.ItemInfo.ItemSlot)) {
                EquipItemFromInventory(newItem);
            }
            bSuccessful = true;
        }
    }
    if (bSuccessful) {
        currentInventoryWeight += itemData.ItemWeight * addeditemstotal;
        OnInventoryChanged.Broadcast(Inventory);
        return addeditemstotal;
    }

    return addeditemstotal;
}

void UACFEquipmentComponent::SetInventoryItemSlotIndex_Implementation(const FInventoryItem& item, int newIndex) {
    if (newIndex < MaxInventorySlots) {
        if (Inventory.Contains(item)) {
            FInventoryItem* _item = Internal_GetInventoryItem(item);
            if (_item->InventoryIndex != newIndex) {
                if (IsSlotEmpty(newIndex))
                    _item->InventoryIndex = newIndex;
                else {
                    FInventoryItem _itemTemp;
                    if (GetItemByInventoryIndex(newIndex, _itemTemp)) {
                        FInventoryItem* _itemDestination = Internal_GetInventoryItem(_itemTemp);
                        _itemDestination->InventoryIndex = _item->InventoryIndex;
                        _item->InventoryIndex = newIndex;
                    }
                }
            }
        }
    }
}

bool UACFEquipmentComponent::SetInventoryItemSlotIndex_Validate(const FInventoryItem& item, int newIndex) {
    return true;
}


TArray<FInventoryItem*> UACFEquipmentComponent::FindItemsByClass(const TSubclassOf<AACFItem>& itemToFind)
{
    TArray<FInventoryItem*> foundItems;

    for (int i = 0; i < Inventory.Num(); i++) {
        if (Inventory[i].itemClass == itemToFind) {
            foundItems.Add(&Inventory[i]);
        }
    }
    return foundItems;
}


void UACFEquipmentComponent::BeginDestroy()
{
    //Internal_DestroyEquipment();
    Super::BeginDestroy();

}



void UACFEquipmentComponent::AttachWeaponOnBody(AACFWeapon* WeaponToEquip)
{
    if (MainCharacterMesh) {
        const FName socket = WeaponToEquip->GetOnBodySocketName();

        if (socket != NAME_None) {
            WeaponToEquip->AttachToComponent(MainCharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, socket);
            WeaponToEquip->Internal_OnWeaponSheathed();
        } else {
            UE_LOG(LogTemp, Log, TEXT("Remember to setup sockets in your weapon! - ACFEquipmentComp"));
        }
    }
}

void UACFEquipmentComponent::AttachWeaponOnHand(AACFWeapon* localWeapon)
{
    const FName socket = localWeapon->GetEquippedSocketName();
    if (socket != NAME_None) {
        localWeapon->AttachToComponent(MainCharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, socket);
        localWeapon->Internal_OnWeaponUnsheathed();
    } else {
        UE_LOG(LogTemp, Log, TEXT("Remember to setup sockets in your weapon! - ACFEquipmentComp"));
    }
}

void UACFEquipmentComponent::AddSkeletalMeshComponent_Implementation(TSubclassOf<AACFArmor> ArmorClass)
{
	if (!ArmorClass) {
		return;
	}

    if (!CharacterOwner) {
        CharacterOwner = Cast<ACharacter>(GetOwner());
    }
    if (!CharacterOwner) {
        return;
    }
	FModularPart outMesh;
	const AACFArmor* ArmorToAdd = Cast<AACFArmor>(ArmorClass->GetDefaultObject());

	if (!ArmorToAdd->GetMeshComponent()) {
		return;
	}

    if (GetModularMesh(ArmorToAdd->GetItemSlot(), outMesh) && outMesh.meshComp) {
		outMesh.meshComp->SetSkeletalMesh(ArmorToAdd->GetMeshComponent()->SkeletalMesh);
		outMesh.meshComp->SetMasterPoseComponent(CharacterOwner->GetMesh());
	}
	else {
		USkeletalMeshComponent* NewComp = NewObject<USkeletalMeshComponent>(CharacterOwner, ArmorToAdd->GetItemSlot().GetTagName());

		NewComp->RegisterComponent(); 
		NewComp->SetWorldLocation(FVector::ZeroVector);
		NewComp->SetWorldRotation(FRotator::ZeroRotator);
		NewComp->AttachToComponent(CharacterOwner->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		NewComp->SetSkeletalMesh(ArmorToAdd->GetMeshComponent()->SkeletalMesh);
		NewComp->SetMasterPoseComponent(CharacterOwner->GetMesh());
		FModularPart mod;
		mod.meshComp = NewComp;
		mod.ItemSlot = ArmorToAdd->GetItemSlot();
		ModularMeshes.Add(mod);
	}
}

bool UACFEquipmentComponent::AddSkeletalMeshComponent_Validate(TSubclassOf<AACFArmor> ArmorClass)
{
    return true;
}

void UACFEquipmentComponent::UseEquippedItemBySlot_Implementation(FGameplayTag ItemSlot)
{
    if (!UACFItemSystemFunctionLibrary::IsValidItemSlotTag(ItemSlot)) {
        UE_LOG(LogTemp, Log, TEXT("Invalid item Slot Tag!!! - ACFEquipmentComp"));
        return;
    }

    if (ItemSlot == CurrentlyEquippedSlotType) {
        SheathCurrentWeapon();
        return;
    }

    FEquippedItem EquipSlot;

    if (GetEquippedItemSlot(ItemSlot, EquipSlot)) {
        AACFWeapon* localWeapon = Cast<AACFWeapon>(EquipSlot.Item);
        if (localWeapon) {
            if (localWeapon->GetHandleType() == EHandleType::OffHand) {
                if (Equipment.MainWeapon && Equipment.MainWeapon->GetHandleType() == EHandleType::OneHanded) {
                     Equipment.SecondaryWeapon = localWeapon;
                }
                else {
					UE_LOG(LogTemp, Log, TEXT("Impossible to equip Offhand! - ACFEquipmentComp"));
                    return;
                }
            }
            else {
				if (Equipment.MainWeapon) {
					SheathCurrentWeapon();
				}
				Equipment.MainWeapon = localWeapon;
				CurrentlyEquippedSlotType = ItemSlot;
            }
            AttachWeaponOnHand(localWeapon);
        } else if (EquipSlot.Item->IsA(AACFConsumable::StaticClass())) {
            AACFConsumable* _consumable = Cast<AACFConsumable>(EquipSlot.Item);
            _consumable->Internal_UseItem(CharacterOwner);
            if (_consumable->bConsumeOnUse) {
                EquipSlot.InventoryItem.Count -= 1;
                RemoveItem(EquipSlot.InventoryItem, 1);
            }
        }
        OnEquipmentChanged.Broadcast(Equipment);
    }
}

bool UACFEquipmentComponent::UseEquippedItemBySlot_Validate(FGameplayTag WeaponSlot)
{
    return true;
}

void UACFEquipmentComponent::UnequipItemBySlot_Implementation(FGameplayTag itemSlot)
{
    if (!UACFItemSystemFunctionLibrary::IsValidItemSlotTag(itemSlot)) {
        UE_LOG(LogTemp, Log, TEXT("Invalid item Slot Tag!!! - ACFEquipmentComp"));
        return;
    }

    if (itemSlot == CurrentlyEquippedSlotType) {
        SheathCurrentWeapon();
      //  return;
    }

    FEquippedItem EquipSlot;
    if (GetEquippedItemSlot(itemSlot, EquipSlot)) {
        RemoveItemFromEquipment(EquipSlot);
    }
}

bool UACFEquipmentComponent::UnequipItemBySlot_Validate(FGameplayTag itemSlot)
{
    return true;
}

void UACFEquipmentComponent::SheathCurrentWeapon_Implementation()
{
    if (Equipment.MainWeapon) {
        FName socket = Equipment.MainWeapon->GetOnBodySocketName();
        if (socket != NAME_None) {
            Equipment.MainWeapon->AttachToComponent(MainCharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, socket);
            Equipment.MainWeapon->Internal_OnWeaponSheathed();
            Equipment.MainWeapon = nullptr;
        } else {
            UE_LOG(LogTemp, Log, TEXT("Remember to setup sockets in your weapon! - ACFEquipmentComp"));
        }

        if (Equipment.SecondaryWeapon) {
            socket = Equipment.SecondaryWeapon->GetOnBodySocketName();
            if (socket != NAME_None) {
                Equipment.SecondaryWeapon->AttachToComponent(MainCharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, socket);
                Equipment.SecondaryWeapon->Internal_OnWeaponSheathed();
                Equipment.SecondaryWeapon = nullptr;

            } else {
                UE_LOG(LogTemp, Log, TEXT("Remember to setup sockets in your weapon! - ACFEquipmentComp"));
            }
        }
        Equipment.SecondaryWeapon = nullptr;
    }

    OnEquipmentChanged.Broadcast(Equipment);
    CurrentlyEquippedSlotType = UACFItemSystemFunctionLibrary::GetItemSlotTagRoot();
}

bool UACFEquipmentComponent::SheathCurrentWeapon_Validate()
{
    return true;
}

void UACFEquipmentComponent::EquipItemFromInventory_Implementation(const FInventoryItem& inItem)
{
    FInventoryItem item;

    if (Inventory.Contains(inItem.GetItemGuid())) {
        item = *(Internal_GetInventoryItemByGuid(inItem.GetItemGuid()));
    } else {
        return;
    }

    if (!CanBeEquipped(item.itemClass)) {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    UWorld* world = GetWorld();

    if (!world) {
        return;
    }

    AACFItem* itemInstance = world->SpawnActor<AACFItem>(item.itemClass, FVector(0.f), FRotator(0), SpawnParams);

    if (!itemInstance) {
        UE_LOG(LogTemp, Error, TEXT("Impossible to spawn item!!! - ACFEquipmentComp"));
        return;
    }

    UnequipItemBySlot(item.ItemInfo.ItemSlot);

    AACFEquippableItem* equippable = Cast<AACFEquippableItem>(itemInstance);
    if (equippable) {
        equippable->Internal_OnEquipped(CharacterOwner);

        AACFWeapon* WeaponToEquip = Cast<AACFWeapon>(equippable);
        if (WeaponToEquip) {
            AttachWeaponOnBody(WeaponToEquip);
        }

        AACFArmor* ArmorToEquip = Cast<AACFArmor>(equippable);
        if (ArmorToEquip) {
            ArmorToEquip->SetActorHiddenInGame(true);
            AddSkeletalMeshComponent(ArmorToEquip->GetClass());
        }
        AACFProjectile* proj = Cast<AACFProjectile>(equippable);
		if (proj) {
            proj->SetActorHiddenInGame(true);
		}

        AACFAccessory* itemToEquip = Cast<AACFAccessory>(equippable);
        if (itemToEquip) {
            itemToEquip->AttachToComponent(MainCharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale,
                itemToEquip->GetAttachmentSocket());
        }
    }
    MarkItemOnInventoryAsEquipped(item, true);
    Equipment.EquippedItems.Add(FEquippedItem(item, itemInstance));
    OnEquipmentChanged.Broadcast(Equipment);
}

bool UACFEquipmentComponent::EquipItemFromInventory_Validate(const FInventoryItem& item)
{
    return true;
}

void UACFEquipmentComponent::DropItemByInventoryIndex_Implementation(int32 itemIndex, int32 count)
{
    if (Inventory.IsValidIndex(itemIndex)) {
        DropItem(Inventory[itemIndex], count);
    }
}

bool UACFEquipmentComponent::DropItemByInventoryIndex_Validate(int32 itemIndex, int32 count)
{
    return true;
}

void UACFEquipmentComponent::RemoveItemFromEquipment(const FEquippedItem& equippedItem)
{
    if (!equippedItem.Item) {
        return;
    }

    AACFEquippableItem* equippable = Cast<AACFEquippableItem>(equippedItem.Item);

    if (equippable) {
        equippable->Internal_OnUnEquipped();
        if (equippable->IsA(AACFArmor::StaticClass())) {
            Internal_OnArmorUnequipped(equippedItem.Item->GetItemSlot());
        }
    }

    const int32 index = Equipment.EquippedItems.IndexOfByKey(equippedItem.Item->GetItemSlot());
    MarkItemOnInventoryAsEquipped(equippedItem.InventoryItem, false);
    equippedItem.Item->Destroy();
    Equipment.EquippedItems.RemoveAt(index); 
  
    OnEquipmentChanged.Broadcast(Equipment);
}

void UACFEquipmentComponent::MarkItemOnInventoryAsEquipped(const FInventoryItem& item, bool bIsEquipped)
{
    FInventoryItem* _item = Internal_GetInventoryItem(item);
    if (_item) {
        _item->bIsEquipped = bIsEquipped;
    }
}

FInventoryItem* UACFEquipmentComponent::Internal_GetInventoryItem(const FInventoryItem& item)
{
    return Inventory.FindByKey(item);
}

FInventoryItem* UACFEquipmentComponent::Internal_GetInventoryItemByGuid(const FGuid& itemToSearch)
{
    return Inventory.FindByKey(itemToSearch);
}


bool UACFEquipmentComponent::GetItemByGuid(const FGuid& itemGuid, FInventoryItem& outItem) const
{
    if (Inventory.Contains(itemGuid)) {
        outItem = *Inventory.FindByKey(itemGuid);
        return true;
    }
    return false;
}

void UACFEquipmentComponent::GetAllItemsOfClassInInventory(TSubclassOf<AACFItem> itemClass, TArray<FInventoryItem>& outItems) const
{
	outItems.Empty();
	for (int i = 0; i < Inventory.Num(); i++) {
		if (Inventory[i].itemClass == itemClass) {
            outItems.Add(Inventory[i]);
		}
	}
}

bool UACFEquipmentComponent::FindFirstItemOfClassInInventory(TSubclassOf<AACFItem> itemClass, FInventoryItem& outItem) const
{
	for (int i = 0; i < Inventory.Num(); i++) {
		if (Inventory[i].itemClass == itemClass) {
			outItem = Inventory[i];
            return true;
		}
	}
    return false;
}

bool UACFEquipmentComponent::GetEquippedItemSlot(const FGameplayTag& itemSlot, FEquippedItem& outSlot) const
{
    if (Equipment.EquippedItems.Contains(itemSlot)) {
        int32 index = Equipment.EquippedItems.IndexOfByKey(itemSlot);
        outSlot = Equipment.EquippedItems[index];
        return true;
    }
    return false;
}

bool UACFEquipmentComponent::GetModularMesh(FGameplayTag itemSlot, FModularPart& outMesh) const
{
    const FModularPart* slot = ModularMeshes.FindByKey(itemSlot);
    if (slot) {
        outMesh = *slot;
        return true;
    }
    return false;
}

bool UACFEquipmentComponent::HasAnyItemInEquipmentSlot(FGameplayTag itemSlot) const
{
    return Equipment.EquippedItems.Contains(itemSlot);
}


void UACFEquipmentComponent::UseConsumableOnActorBySlot_Implementation(FGameplayTag itemSlot, ACharacter* target)
{
	if (!UACFItemSystemFunctionLibrary::IsValidItemSlotTag(itemSlot)) {
		UE_LOG(LogTemp, Log, TEXT("Invalid item Slot Tag!!! - ACFEquipmentComp"));
		return;
	}

	FEquippedItem EquipSlot;
	if (GetEquippedItemSlot(itemSlot, EquipSlot)) {
	 if (EquipSlot.Item->IsA(AACFConsumable::StaticClass())) {
			AACFConsumable* _consumable = Cast<AACFConsumable>(EquipSlot.Item);
			_consumable->Internal_UseItem(target);
			if (_consumable->bConsumeOnUse) {
				EquipSlot.InventoryItem.Count -= 1;
				RemoveItem(EquipSlot.InventoryItem, 1);
			}
            OnEquipmentChanged.Broadcast(Equipment);
		}
	}
}


bool UACFEquipmentComponent::UseConsumableOnActorBySlot_Validate(FGameplayTag itemSlot, ACharacter* target)
{
    return true;
}

void UACFEquipmentComponent::SetDamageActivation(bool isActive, const TArray<FName>& traceChannels, bool isSecondaryWeapon /*= false*/)
{
    AACFMeleeWeapon* weapon;
    if (isSecondaryWeapon && Equipment.SecondaryWeapon) {
        weapon = Cast<AACFMeleeWeapon>(Equipment.SecondaryWeapon);
    } else {
        weapon = Cast<AACFMeleeWeapon>(Equipment.MainWeapon);
    }

    if (weapon) {
        if (isActive) {
            weapon->StartWeaponSwing(traceChannels);
        } else {
            weapon->StopWeaponSwing();
        }
    }
}

bool UACFEquipmentComponent::CanSwitchToRanged()
{
    for (const auto& weap : Equipment.EquippedItems) {
        if (weap.Item->IsA(AACFRangedWeapon::StaticClass())) {
            return true;
        }
    }
    return false;
}

bool UACFEquipmentComponent::CanSwitchToMelee()
{
    for (const auto& weap : Equipment.EquippedItems) {
        if (weap.Item->IsA(AACFMeleeWeapon::StaticClass())) {
            return true;
        }
    }
    return false;
}

bool UACFEquipmentComponent::HasOnBodyAnyWeaponOfType(TSubclassOf<AACFWeapon> weaponClass) const
{
    for (const auto& weapon : Equipment.EquippedItems) {
        if (weapon.Item->IsA(weaponClass)) {
            return true;
        }
    }
    return false;
}

void UACFEquipmentComponent::InitializeInventoryAndEquipment()
{

    if (GetOwner()->HasAuthority()) {
        Inventory.Empty();
        currentInventoryWeight = 0.f;
        for (const FStartingItem& item : StartingItems) {
            Internal_AddItem(item, item.bAutoEquip, item.DropChancePercentage);
            if (Inventory.Num() > MaxInventorySlots) {
                UE_LOG(LogTemp, Log, TEXT("Invalid Inventory setup, too many slots on character!!! - ACFEquipmentComp"));
            }
        }
    }
}

void UACFEquipmentComponent::DestroyEquippedItems_Implementation()
{
    Internal_DestroyEquipment();

    if (bDropItemsOnDeath && Inventory.Num() > 0) {
        for (int32 Index = Inventory.Num() - 1; Index >= 0; --Index) {
            if (Inventory.IsValidIndex(Index) && Inventory[Index].ItemInfo.bDroppable && Inventory[Index].DropChancePercentage > FMath::RandRange(0.f, 100.f)) {
                DropItem(Inventory[Index], Inventory[Index].Count);
            }
        }
    }
}

bool UACFEquipmentComponent::DestroyEquippedItems_Validate()
{
    return true;
}



void UACFEquipmentComponent::SpawnWorldItem(int32 count, TSubclassOf<AACFItem> item)
{
    if (count >= 1 && CharacterOwner) {
        FVector startLoc = CharacterOwner->GetCharacterMovement()->GetActorFeetLocation();
        FBaseItem tobeAdded(item, count);

        UACFItemSystemFunctionLibrary::SpawnWorldItemNearLocation(this, tobeAdded, startLoc);
    }
}

void UACFEquipmentComponent::DestroyEquipment()
{
	for (auto& equip : Equipment.EquippedItems) {
		AACFEquippableItem* equippable = Cast<AACFEquippableItem>(equip.Item);
		if (equippable) {
			equippable->Internal_OnUnEquipped();
		}
		equip.Item->Destroy();
	}
}

int32 UACFEquipmentComponent::NumberOfItemCanTake(const TSubclassOf<AACFItem>& itemToCheck)
{
    int32 addeditemstotal = 0;
    TArray<FInventoryItem*> outItems = FindItemsByClass(itemToCheck);
    FItemDescriptor itemInfo;
    UACFItemSystemFunctionLibrary::GetItemData(itemToCheck, itemInfo);
    const float MaxByWeight = (MaxInventoryWeight - currentInventoryWeight) / itemInfo.ItemWeight;
    const int32 maxAddableByWeight = FMath::TruncToInt(MaxByWeight);
    const int32 FreeSpaceInInventory = MaxInventorySlots - Inventory.Num();
    int32 maxAddableByStack = FreeSpaceInInventory * itemInfo.MaxInventoryStack;
    //IF WE ALREADY HAVE SOME ITEMS LIKE THAT, INCREMENT ACTUAL VALUE
    if (outItems.Num() > 0) {
        for (const auto& outItem : outItems) {
            maxAddableByStack += itemInfo.MaxInventoryStack - outItem->Count;
        }
    }
    addeditemstotal = FGenericPlatformMath::Min(maxAddableByStack, maxAddableByWeight);
    return addeditemstotal;
}

void UACFEquipmentComponent::Internal_DestroyEquipment()
{
    for (auto& weap : Equipment.EquippedItems) {
        if (weap.Item) {
            weap.Item->SetLifeSpan(1.f);
        }
    }
}

// void UACFEquipmentComponent::MoveItemToAnotherInventory_Implementation(UACFEquipmentComponent* OtherEquipmentComponent, class AACFItem* itemToMove, int32 count /*= 1*/)
// {
//     if (!itemToMove) {
//         return;
//     }
//     if (itemToMove->GetItemInfo().MaxInventoryStack > 1) {
//         int32 howmanycantake = OtherEquipmentComponent->NumberOfItemCanTake(itemToMove);
//         if (howmanycantake < count) {
//             count = howmanycantake;
//         }
//         int NumberToRemove = OtherEquipmentComponent->Internal_AddItem(itemToMove, count);
//         //int NumberToRemove=OtherEquipmentComponent->AddItemToInventoryByClass(itemToMove->GetClass(), count, false);
//         RemoveItem(itemToMove, NumberToRemove);
//     } else {
//         int NumberToRemove = OtherEquipmentComponent->Internal_AddItem(itemToMove, 1);
//         if (NumberToRemove == 1) {
//             RemoveItem(itemToMove);
//         }
//     }
// }

// bool UACFEquipmentComponent::MoveItemToAnotherInventory_Validate(UACFEquipmentComponent* OtherEquipmentComponent, class AACFItem* itemToMove, int32 count /*= 1*/)
// {
//     return true;
// }
