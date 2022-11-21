// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/PlayerController.h>
#include "Actors/ACFCharacter.h"
#include <GameplayTagContainer.h>
#include "ACFPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPossessedCharacterChanged, const class AACFCharacter*, character);


UCLASS()
class ASCENTCOMBATFRAMEWORK_API AACFPlayerController : public APlayerController
{
	GENERATED_BODY()


public: 

	AACFPlayerController();


	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE class AACFCharacter* GetPossessedACFCharacter() const { return PossessedCharacter; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE FRotator GetCameraInput() const { return cameraInput; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE float GetSecondsFromLastCameraInput() const { return secondsFromLastCameraInput; }

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE ETeam GetCombatTeam()const { return CombatTeam; }

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = ACF)
	void SetCombatTeam(const ETeam& newTeam);

	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnPossessedCharacterChanged OnPossessedCharacterChanged;

	virtual void AddYawInput(float Val) override;
	virtual void AddPitchInput(float Val) override;
	virtual void AddRollInput(float Val) override;

protected:

	virtual void BeginPlay() override;

	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnPossess(APawn* aPawn) override;

	void EnableCharacterComponents(bool bEnabled);

	virtual void OnUnPossess() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ACF)
	ETeam CombatTeam = ETeam::ETeam1;

	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_PossessedEntity, Category = ACF)
	class AACFCharacter* PossessedCharacter;

	IACFEntityInterface* PossessedEntity;
private: 

	FRotator cameraInput;

	float secondsFromLastCameraInput = 0.f;

	FDateTime lastInput;

	UFUNCTION()
	void OnRep_PossessedEntity();

	void HandleNewEntityPossessed();

};
