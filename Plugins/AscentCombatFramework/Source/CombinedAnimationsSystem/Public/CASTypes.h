// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include <GameFramework/Character.h>
#include "GameplayTagContainer.h"
#include "CASTypes.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombinedAnimationStarted, const FGameplayTag&, animTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombinedAnimationEnded, const FGameplayTag&, animTag);

UENUM(BlueprintType)
enum class ERelativeDirection : uint8
{
	EAny	            UMETA(DisplayName = "Any"),
	EFrontal			UMETA(DisplayName = "Facing Each Other Only"),
	EOpposite			UMETA(DisplayName = "From Behind"),
};

USTRUCT(BlueprintType)
struct FCombinedAnimsMaster : public FTableRowBase
{
	GENERATED_BODY()

public:
	FCombinedAnimsMaster():MasterAnimMontage(nullptr){}
	/*The tag of this combind anim */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CAS")
	FGameplayTag AnimTag;

	/*The montage to be playd on master */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CAS")
	class UAnimMontage* MasterAnimMontage;

	/*Max distance between master and slave characters to start snapping throught 
	the start position*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CAS")
	float MaxDistanceToStartSnapping = 450.f;

	/*After the snap timeout, the animation combined will only start if we
	are closer then this distance to the target starting position*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CAS")
	float MaxDistanceToStartCombinedAnimation = 60.f;
	
	/*If the slave is not in this relative direction to the master, 
	the animation combined cannot start. Select Any to ignore this check*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CAS")
	ERelativeDirection SlaveRequiredDirection = ERelativeDirection::EAny;

	/*Once the animation starts, the slaved character will be rotated to this direction.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CAS")
	ERelativeDirection SlaveForcedDirection = ERelativeDirection::EFrontal;

};

USTRUCT(BlueprintType)
struct FCombinedAnimsSlave : public FTableRowBase
{
	GENERATED_BODY()

public:
	FCombinedAnimsSlave():MasterAnimMontage(nullptr){}
	/*The tag of this combind anim */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CAS")
	FGameplayTag AnimTag;

	/*The montage to be playd on slave */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CAS")
	class UAnimMontage* MasterAnimMontage;

};

USTRUCT(BlueprintType)
struct FCurrentCombinedAnim 
{
	GENERATED_BODY()

public:

	FCurrentCombinedAnim():AnimSlave(nullptr){};

	FCurrentCombinedAnim(const FCombinedAnimsMaster& inMasterConfig, const FGameplayTag& inTag, ACharacter* inCharacterRef) {
		MasterAnimConfig = inMasterConfig;
		AnimTag = inTag;
		AnimSlave = inCharacterRef;
	}

	/*The */
	UPROPERTY(BlueprintReadOnly, Category = "CAS")
	FCombinedAnimsMaster MasterAnimConfig;

	UPROPERTY(BlueprintReadOnly, Category = "CAS")
	FGameplayTag AnimTag;

	UPROPERTY(BlueprintReadOnly, Category = "CAS")
	ACharacter* AnimSlave;
};
/**
 * 
 */
UCLASS()
class COMBINEDANIMATIONSSYSTEM_API UCASTypes : public UObject
{
	GENERATED_BODY()
	
};
