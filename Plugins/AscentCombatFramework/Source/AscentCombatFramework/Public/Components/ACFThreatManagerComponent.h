// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ACFThreatManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewMaxThreateningActor, class AActor*, threatening);

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class ASCENTCOMBATFRAMEWORK_API UACFThreatManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UACFThreatManagerComponent();

	/*Adds threat for the provided actor*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	void AddThreat(class AActor* threatening, float threat);

	/*Remove threat for the provided actor*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	void RemoveThreat(class AActor* threatening, float threat);

	/*Returns the actor with higher threat*/
	UFUNCTION(BlueprintPure, Category = ACF)
	class AActor* GetActorWithHigherThreat();

	/*Returns the threat multiplier for provided actor*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	float GetThreatMultiplierForActor(AActor* threatening);

	/*Returns true if this actor has threat >=0*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	bool IsThreatening(class AActor* threatening) const;

	/*get the default threat to be added for the percepted actor*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	float GetDefaultThreatForActor(AActor* threatening);

	/*Remove the provided actor */
	UFUNCTION(BlueprintCallable, Category = ACF)
	void RemoveThreatening(AActor* threatening);

	/*called when there is a new highest threaning actor in the list*/
	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnNewMaxThreateningActor OnNewMaxThreateningActor;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	TMap<TSubclassOf<AActor>, float> DefaultThreatMap;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	TMap<TSubclassOf<AActor>, float> ThreatMultiplierMap;

private: 

	UPROPERTY()
	TMap<class AActor*, float> ThreatMap;

	UPROPERTY()
	class AActor* maxThreatening;

	void UpdateMaxThreat();
};
