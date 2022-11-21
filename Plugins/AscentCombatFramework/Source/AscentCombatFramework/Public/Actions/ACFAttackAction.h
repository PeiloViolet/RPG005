// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Actions/ACFBaseAction.h"
#include "ACFAttackAction.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EComboCycleType : uint8
{
	ELastCombo = 0       UMETA(DisplayName = "LastCombo"),
	EOverall       UMETA(DisplayName = "Overall"),
	ENor           UMETA(DisplayName = "Nor"),
};

UENUM(BlueprintType)
enum class EComboIndexType : uint8
{
	ESections = 0       UMETA(DisplayName = "Sections"),
	ESectionsSequence       UMETA(DisplayName = "SectionsSequence"),

};

UCLASS()
class ASCENTCOMBATFRAMEWORK_API UACFAttackAction : public UACFBaseAction
{
	GENERATED_BODY()

public: 

	UACFAttackAction();

	bool bIsEnterCombo = false;
	
	uint8 CurrentComboIndex = 0;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = RPG)
	EComboCycleType ComboCyclesType{EComboCycleType::ELastCombo};

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "ComboCyclesType == EComboCycleType::ELastCombo"), Category = RPG)
	uint8 StartCyclesComboIndex{0};

	UPROPERTY(EditDefaultsOnly, Category = RPG)
	uint8 EndComboIndex{2};
	
	UPROPERTY(EditDefaultsOnly, Category = RPG)
	EComboIndexType ComboIndexType{EComboIndexType::ESections};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "ComboIndexType == EComboIndexType::ESectionsSequence"), Category = RPG)
	TArray<FName> ComboSequenceNames{TEXT("Combo01"), TEXT("Combo02")};
	
	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE uint8 GetComboCounter() { return CurrentComboIndex; }

	UFUNCTION(BlueprintCallable, Category = ACF)
	void ResetComboCounter() { CurrentComboIndex = 0; }
	
	UFUNCTION(BlueprintCallable, Category = ACF)
	void SetComboCounter(uint8 ComboCounter) { CurrentComboIndex = ComboCounter; }

protected:

	virtual void OnActionStarted_Implementation() override;

	virtual void OnActionEnded_Implementation() override;

	virtual FName GetMontageSectionName_Implementation() override;

	virtual void OnSubActionStateEntered_Implementation() override;

	virtual void OnSubActionStateExited_Implementation() override;

	virtual void OnActionTransition_Implementation(class UACFBaseAction* previousState) override;

	
	
	
	
	UPROPERTY(EditDefaultsOnly, Category = ACF)
	FSnapConfiguration AttackSnapConfig;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	float RotationRateMultiplierDuringPreWinding = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	float RotationRateMultiplierDuringSwing = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = ACF)
	float RotationRateMultiplierAfterWinding = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	EDamageActivationType DamageToActivate;

	UPROPERTY(EditDefaultsOnly, Category = ACF)
	TArray<FName> TraceChannels;


private :

	float oldRotationRate;

	

};
