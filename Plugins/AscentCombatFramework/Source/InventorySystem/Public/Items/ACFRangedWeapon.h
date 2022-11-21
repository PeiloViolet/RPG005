// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Items/ACFWeapon.h"
#include "Components/ACFShootingComponent.h"
/*#include "Game/ACFTypes.h"*/
#include "ACFRangedWeapon.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API AACFRangedWeapon : public AACFWeapon
{
	GENERATED_BODY()

public:

	AACFRangedWeapon();

	/*Shoots at owner's eyesight*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	virtual void Shoot(const FRotator& deltaRot = FRotator::ZeroRotator, float  charge = 1.f, TSubclassOf<class AACFProjectile> projectileOverride = nullptr);

	/*Shoots at the target actor*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	virtual void ShootAtActor(const AActor* target, float randomDeviation = 5.f, float charge = 1.f, TSubclassOf<class AACFProjectile> projectileOverride = nullptr);

	/*Shoots at the provided direction */
	UFUNCTION(BlueprintCallable, Category = ACF)
	void ShootAtDirection(const FRotator& direction, float charge = 1.f, TSubclassOf<class AACFProjectile> projectileOverride = nullptr);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void LineTraceShoot(const FRotator& deltaRot = FRotator::ZeroRotator);

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE float GetProjectileSpeed() const { 
	return ShootingComp ? ShootingComp->GetShootingSpeed() : -1.f; }

	 void OnItemEquipped_Implementation() override;


	 UFUNCTION(BlueprintPure, Category = ACF)
	 FORCEINLINE EShootingType GetShootingType() const {
		 return ShootingType;
	 }

	 UFUNCTION(BlueprintPure, Category = ACF)
	 FORCEINLINE UACFShootingComponent* GetShootingComponent() const {
		 return ShootingComp;
	 }

	 UFUNCTION(BlueprintPure, Category = ACF)
	 FORCEINLINE bool CanShoot() const {
		 return ShootingComp->CanShoot();
	 }

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = ACF)
	UACFShootingComponent* ShootingComp;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = ACF)
	EShootingType ShootingType = EShootingType::EProjectile;

private: 
	
};
   