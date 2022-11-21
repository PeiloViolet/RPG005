// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Items/ACFItem.h"
#include "ARSTypes.h"
#include "ACFItemSystemFunctionLibrary.generated.h"


/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UACFItemSystemFunctionLibrary : public UBlueprintFunctionLibrary {
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = ACFItemLibrary)
    static AACFWorldItem* SpawnWorldItemNearLocation(UObject* WorldContextObject, const FBaseItem& ContainedItem, 
        const FVector& location, float acceptanceRadius = 100.f);

	UFUNCTION(BlueprintCallable, Category = ACFItemLibrary)
    static bool GetItemData(const TSubclassOf<class AACFItem>& item, FItemDescriptor &outData);

	UFUNCTION(BlueprintCallable, Category = ACFItemLibrary)
	static bool GetEquippableAttributeSetModifier(const TSubclassOf<class AACFItem>& itemClass, FAttributesSetModifier& outModifier);

	UFUNCTION(BlueprintCallable, Category = ACFItemLibrary)
	static bool GetEquippableAttributeRequirements(const TSubclassOf<class AACFItem>& itemClass, TArray<FAttribute>& outAttributes);

	UFUNCTION(BlueprintCallable, Category = ACFItemLibrary)
	static bool GetConsumableTimedAttributeSetModifier(const TSubclassOf<class AACFItem>& itemClass, TArray<FTimedAttributeSetModifier>& outModifiers);
	
	UFUNCTION(BlueprintCallable, Category = ACFItemLibrary)
	static bool GetConsumableStatModifier(const TSubclassOf<class AACFItem>& itemClass, TArray <FStatisticModifier>& outModifiers);

	UFUNCTION(BlueprintCallable, Category = ACFLibrary)
	static FGameplayTag GetItemTypeTagRoot();

	UFUNCTION(BlueprintCallable, Category = ACFLibrary)
	static FGameplayTag GetItemSlotTagRoot();

	UFUNCTION(BlueprintCallable, Category = ACFLibrary)
	static bool IsValidItemTypeTag(FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category = ACFLibrary)
	static bool IsValidItemSlotTag(FGameplayTag TagToCheck);


	UFUNCTION(BlueprintPure, Category = ACFLibrary)
	static TSubclassOf<class AACFWorldItem> GetDefaultWorldItemClass();
};
