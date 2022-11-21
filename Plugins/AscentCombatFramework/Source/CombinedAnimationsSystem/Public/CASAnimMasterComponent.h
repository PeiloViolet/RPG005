// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CASTypes.h"
#include "GameplayTagContainer.h"
#include "CASAnimMasterComponent.generated.h"



UCLASS( ClassGroup=(CAS), meta=(BlueprintSpawnableComponent) )
class COMBINEDANIMATIONSSYSTEM_API UCASAnimMasterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCASAnimMasterComponent();

	/*Tries to play the animation tagged as combineAnimationTag with OtherCharacter and returns wathever this 
	operation is successful*/
	UFUNCTION( BlueprintCallable, Category = CAS)
	bool TryPlayCombinedAnimation( class ACharacter* otherCharachter, const FGameplayTag& combineAnimTag);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = CAS)
	void PlayCombinedAnimation(class ACharacter* otherCharachter, const FGameplayTag& combineAnimTag);

	UFUNCTION(BlueprintCallable, Category = CAS)
	bool CanPlayCombinedAnimWithCharacter(ACharacter* otherCharachter, const FGameplayTag& combineAnimTag);

	UFUNCTION(BlueprintPure, Category = CAS)
	FORCEINLINE bool IsPlayingCombinedAnimation() const {
		return bIsPlayingCombAnim;
	}

	UFUNCTION(BlueprintPure, Category = CAS)
	FORCEINLINE ACharacter* GetCharacterOwner() const {
		return characterOwner;
	}

	UFUNCTION(BlueprintPure, Category = CAS)
	FORCEINLINE ACharacter* GetCharacterSlave() const {
		return currentAnim.AnimSlave;
	}

	UPROPERTY(BlueprintAssignable, Category = CAS)
	FOnCombinedAnimationStarted ServerCombinedAnimationStarted;

	UPROPERTY(BlueprintAssignable, Category = CAS)
	FOnCombinedAnimationEnded ServerCombinedAnimationEnded;

	UPROPERTY(BlueprintAssignable, Category = CAS)
	FOnCombinedAnimationStarted OnCombinedAnimationStarted;

	UPROPERTY(BlueprintAssignable, Category = CAS)
	FOnCombinedAnimationEnded OnCombinedAnimationEnded;

	UFUNCTION(BlueprintCallable, Category = CAS)
	ERelativeDirection GetCharacterRelativedDirection( const ACharacter* otherChar) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category = CAS)
	float SnapSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = CAS)
	float SnapTimeout = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = CAS)
	class UDataTable* MasterAnimsConfig;

	virtual bool EvaluateCombinedAnim(const FCombinedAnimsMaster& animConfig, const ACharacter* otherChar) const;

	virtual void OnCombinedAnimStarted(const FGameplayTag& animTag);
	virtual void OnCombinedAnimEnded(const FGameplayTag& animTag);

	FCurrentCombinedAnim currentAnim;

	class ACharacter* characterOwner;

private:

	bool bIsPlayingCombAnim = false;


	FCombinedAnimsMaster* GetCombinedAnimTag(const FGameplayTag& combineAnimTag);
	UFUNCTION()
	void HandleMontageFinished(UAnimMontage* inMontage, bool bInterruptted);

	UFUNCTION()
	void HandleSnapPointReached(bool bSuccess);

	void StartAnim();

	void StartAnimOnSlave();

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = CAS)
	void MulticastPlayAnimMontage(class UAnimMontage* AnimMontage, class ACharacter* character);

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = CAS)
	void DispatchAnimStarted(const FGameplayTag& animTag);

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = CAS)
	void DispatchAnimEnded(const FGameplayTag& animTag);

};
