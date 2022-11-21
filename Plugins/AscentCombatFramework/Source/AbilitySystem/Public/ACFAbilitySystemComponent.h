// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Abilities/ACFAbility.h"
#include "ACFAbilityFunctionLibrary.h"
#include "ACFAbilitySystemComponent.generated.h"



UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class ABILITYSYSTEM_API UACFAbilitySystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFAbilitySystemComponent();


	UFUNCTION(BlueprintCallable, Category = ACF)
	void GrantAbility(const FName& abilityName);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void RemoveAbility(const FName& abilityName);

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool TryUseAbility(const FName& abilityName);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void DeactivateAbility(const FName& abilityName);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void InitAbilities();

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool GrantAndUseAbility(const FName& abilityName);

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool HasAbility(const FName& abilityName) const;

	UFUNCTION(BlueprintCallable, Category = ACF)
	bool HasActiveAbility(const FName& abilityName) const;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	TArray<FName> StartingAbilities;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	bool bAutoInitialize = false;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	bool bAutoActivatePassiveAbilities = false;


private:
	
	UPROPERTY(Savegame)
	TMap<FName, UACFAbility*> currentlyActiveAbilities; 

	UPROPERTY(Savegame)
	TMap<FName, FAbilityConfig> currentlyAvailableAbilities;

		
};
