// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ACFAbility.generated.h"

/**
 * 
 */

UCLASS(Blueprintable, BlueprintType)
class ABILITYSYSTEM_API UACFAbility : public UObject
{
	GENERATED_BODY()

public: 

	UACFAbility(){};

	UWorld* GetWorld() const override;

	friend class UACFAbilitySystemComponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	EAbilityType AbilityType;

	UFUNCTION(BlueprintNativeEvent, Category = ACF)
	bool CanBeActivated();
	virtual bool CanBeActivated_Implementation();


	UFUNCTION(BlueprintNativeEvent, Category = ACF)
	void OnAbilityActivated();
	virtual void OnAbilityActivated_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = ACF)
	void OnAbilityDeactivated();
	virtual void OnAbilityDeactivated_Implementation();

	UFUNCTION(BlueprintPure, Category = ACF)
	class AActor* GetActorOwner() const {
		return actorOwner;
	}

	UFUNCTION(BlueprintPure, Category = ACF)
	EAbilityType GetAbilityType() const {
		return AbilityType;
	}

private:

	class AActor* actorOwner;
	
	void Internal_OnActivated(class AActor* owner);

};



