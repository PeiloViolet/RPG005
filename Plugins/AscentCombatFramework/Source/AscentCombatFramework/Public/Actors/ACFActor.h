// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Game/ACFTypes.h"
#include "Game/ACFDamageType.h"
#include <GenericTeamAgentInterface.h>
#include "Interfaces/ACFEntityInterface.h"
#include "ACFActor.generated.h"

UCLASS()
class ASCENTCOMBATFRAMEWORK_API AACFActor : public AActor, 
	public IGenericTeamAgentInterface, public IACFEntityInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AACFActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*Used to identify who can attack this actor*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ACF)
	ETeam CombatTeam = ETeam::ETeam1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UARSStatisticsComponent* StatisticsComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UACFEffectsManagerComponent* EffetsComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UACFEquipmentComponent* EquipmentComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UACFDamageHandlerComponent* DamageHandlerComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UAudioComponent* AudioComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ACF)
	class UAIPerceptionStimuliSourceComponent* AIPerceptionStimuliSource;

	UFUNCTION(BlueprintNativeEvent, Category = ACF)
		void OnActorDestroyed();
	virtual void OnActorDestroyed_Implementation();

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

public:	

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	//acf entity interface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
		ETeam GetEntityCombatTeam();
	virtual ETeam GetEntityCombatTeam_Implementation() override {
		return CombatTeam;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
		bool IsEntityAlive();
	virtual bool IsEntityAlive_Implementation() override {
		return !bIsDead;
	}

	UFUNCTION(BlueprintPure, Category = ACF)
	FACFDamageEvent GetLastDamageInfo() const;


	UFUNCTION(BlueprintPure, Category = ACF)
		FORCEINLINE class UARSStatisticsComponent* GetStatisticsComponent() const { return StatisticsComp; }

	UFUNCTION(BlueprintPure, Category = ACF)
		FORCEINLINE class UACFEquipmentComponent* GetEquipmentComponent() const { return EquipmentComp; }

	UFUNCTION(BlueprintPure, Category = ACF)
		FORCEINLINE class UACFDamageHandlerComponent* GetDamageHandlerComponent() const { return DamageHandlerComp; }

private: 

	UPROPERTY(Replicated)
	bool bIsDead = false;

	UFUNCTION()
	void HandleDeath();

};
