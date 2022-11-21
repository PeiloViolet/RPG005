// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CASSnapperComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSnapPointReached, bool, bSuccess);

UENUM(BlueprintType)
enum class ESnapType : uint8
{
	EToPoint = 0,
	EToActor,
	EToComponent
};

class AActor;

UCLASS(ClassGroup = (ACF), Blueprintable, meta = (BlueprintSpawnableComponent))
class COMBINEDANIMATIONSSYSTEM_API UCASSnapperComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCASSnapperComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*Moves the owner actor to the snapLocation at the provided speed and triggers OnSnapPointReached
	once it it's distance to the provided point is less than distance tolerance or when timeout fires*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	void SnapToPoint(const FVector& snapLocation, float snapSpeed = 2.f, float distanceTolerance = 5.f, float timeoutTime = 2.f, bool bOrientToSnapPoint = true);

	/*Moves the owner actor to the location of the inActor at the provided speed and triggers OnSnapPointReached
	once it it's distance to the provided actor is less than distance tolerance or when timeout fires*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	void SnapToActor(AActor* inActor, float snapSpeed = 2.f, float distanceTolerance = 5.f, float timeoutTime = 2.f, bool bOrientToSnapPoint = true);

	/*Moves the owner actor to the location of the inActor at the provided speed and triggers OnSnapPointReached
	once it it's distance to the provided actor is less than distance tolerance or when timeout fires*/
	UFUNCTION(BlueprintCallable, Category = ACF)
	void SnapToComponent(USceneComponent* inComponent, float snapSpeed = 2.f, float distanceTolerance = 5.f, float timeoutTime = 2.f, bool bOrientToSnapPoint = true);

	/*Trigger once the snapPoint is reached or timeout occurs*/
	UPROPERTY(BlueprintAssignable, Category = ACF)
	FOnSnapPointReached OnSnapPointReached;

private:

	FVector snapPoint;
	float speed;
	float tolerance;
	float timeout;
	float currentTime = 0.f;

	UPROPERTY()
	AActor* owner;

	UPROPERTY()
	AActor* targetActor;


	UPROPERTY()
	USceneComponent* targetComponent;

	ESnapType snapType;
	bool bOrientToMov;

	bool bIsSnapping = false;

	void TerminateSnap(bool bSuccess);

	void StartSnap();
};
