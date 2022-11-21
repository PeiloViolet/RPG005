// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Items/ACFEquippableItem.h"
#include "ACFArmor.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API AACFArmor : public AACFEquippableItem
{
	GENERATED_BODY()

public:

	AACFArmor();

	UFUNCTION(BlueprintCallable, Category = ACF)
	FORCEINLINE class USkeletalMeshComponent* GetMeshComponent() const { return MeshComp; };


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class USkeletalMeshComponent* MeshComp;
	
};
