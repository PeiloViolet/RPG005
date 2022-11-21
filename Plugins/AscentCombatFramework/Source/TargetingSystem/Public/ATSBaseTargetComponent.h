// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ATSBaseTargetComponent.generated.h"





UCLASS(ClassGroup = (ATS), Blueprintable, meta = (BlueprintSpawnableComponent))
class TARGETINGSYSTEM_API UATSBaseTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UATSBaseTargetComponent();

	/*Forces to target the selected actor*/
	UFUNCTION(BlueprintCallable, Category = "ATS")
	virtual	void SetCurrentTarget(class AActor* target) {
		CurrentTarget = target;
	}


	/*Return the currently focused target*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ATS")
	class AActor* GetCurrentTarget() const { return CurrentTarget; }

	/*Returns if there is a valid target*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ATS")
	bool HasTarget() const { return CurrentTarget != nullptr; }

	/*Return the TargetPointComponent focused of the Current target*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ATS")
	FORCEINLINE	class UATSTargetPointComponent* GetCurrentTargetPoint() const { return CurrentTargetPoint; }

	/*Returns true if the target is in sight with the actual pawn owned by the controller*/
	UFUNCTION(BlueprintPure, Category = ATS)
	bool IsTargetInSight() const;
protected:

	UPROPERTY()
	class AActor* CurrentTarget;

	UPROPERTY()
	class UATSTargetPointComponent* CurrentTargetPoint;



};
