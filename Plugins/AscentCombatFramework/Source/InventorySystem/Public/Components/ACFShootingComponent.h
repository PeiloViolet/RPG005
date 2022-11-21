// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Components/MeshComponent.h>
#include "ACFItemTypes.h"
#include "Items/ACFWeapon.h"
#include "ACFEquipmentComponent.h"
#include "ACFShootingComponent.generated.h"


UCLASS(Blueprintable, ClassGroup = (ACF), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UACFShootingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "ACF" )
	FName ProjectileStartSocket = "ProjectileStart";

	UPROPERTY(EditAnywhere, Category = "ACF")
	struct FWeaponEffects ShootingEffect;

	UPROPERTY(EditDefaultsOnly, Category = "ACF | Ammo")
	bool bConsumeAmmo = true;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bConsumeAmmo"), Category = "ACF | Ammo")
	FGameplayTag AmmoSlot;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bConsumeAmmo"), Category = "ACF | Ammo")
	TArray<TSubclassOf<class AACFProjectile>> AllowedProjectiles;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bConsumeAmmo == false"),  Category = "ACF | Ammo")
	TSubclassOf<class AACFProjectile> ProjectileClassBP;
	
	/*Speed at wich the projectile is shot*/
	UPROPERTY(EditDefaultsOnly, Category = "ACF | Projectile")
	float ProjectileShotSpeed;

private:
	UPROPERTY(Replicated)
	UMeshComponent* shootingMesh;

	UPROPERTY(Replicated)
	APawn* characterOwner;

	void Internal_Shoot(const FTransform& spawnTransform, const FVector& ShotDirection, float charge, TSubclassOf<class AACFProjectile> inProjClass);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Internal_SetupComponent(class APawn* inOwner, class UMeshComponent* inMesh);

	class UACFEquipmentComponent* TryGetEquipment()  const;

	bool TryGetAmmoSlot(FEquippedItem& outSlot) const;
	void RemoveAmmo();

	TSubclassOf<AACFItem> GetBestProjectileToShoot() const;

public:
	// Sets default values for this component's properties
	UACFShootingComponent();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = ACF)
	void SetupShootingComponent(class APawn* inOwner, class UMeshComponent* inMesh); 

	UFUNCTION(BlueprintCallable, Category = ACF)
    void ReinitializeShootingComponent(class APawn* inOwner, class UMeshComponent* inMesh, FName inStartSocket, struct FWeaponEffects inShootingFX) {
		shootingMesh = inMesh;
		characterOwner = inOwner;
		ProjectileStartSocket = inStartSocket;
		ShootingEffect = inShootingFX;
	}

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE float GetProjectileSpeed() const { return ProjectileShotSpeed; }

	UFUNCTION(BlueprintCallable, Category = ACF)
	void ShootAtActor(const AActor* target, float randomDeviation = 5.f, float charge = 1.f, TSubclassOf<class AACFProjectile> projectileOverride = nullptr);

	UFUNCTION(BlueprintCallable, Category = ACF)
	void ShootAtDirection(const FRotator& direction, float charge = 1.f, TSubclassOf<class AACFProjectile> projectileOverride = nullptr);

 	UFUNCTION(BlueprintCallable, Category = ACF)
 	void LineTraceShootAtDirection(const FRotator& direction);

	UFUNCTION(BlueprintPure, Category = ACF)
	bool CanShoot() const;

	UFUNCTION(BlueprintPure, Category = ACF)
	bool TryGetAllowedAmmoFromInventory(FInventoryItem& outAmmoSlot) const;

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE float GetShootingSpeed() const {
		return ProjectileShotSpeed;
	}

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE FVector GetShootingSocketPosition() const {
		return shootingMesh->GetSocketLocation(ProjectileStartSocket);
	}

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE FName GetProjectileStartSocketName() const {
		return ProjectileStartSocket;
	}

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE UMeshComponent* GetShootingMesh() const {
		return shootingMesh;
	}

	UFUNCTION(BlueprintPure, Category = ACF)
	FORCEINLINE TArray<TSubclassOf<AACFProjectile>> GetAllowedProjectiles() const {
		return AllowedProjectiles;
	}

	UFUNCTION(NetMulticast, WithValidation, Reliable, Category = ACF)
	void PlayMuzzleEffect();
};
