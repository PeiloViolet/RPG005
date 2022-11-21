#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/AlsMovementSettings.h"
#include "Utility/AlsGameplayTags.h"
#include "AlsCharacterMovementComponent.generated.h"

using FAlsPhysicsRotationDelegate = TMulticastDelegate<void(float DeltaTime)>;

/**
 * @brief 客户端移动网络同步数据
 */
class RPG005_API FAlsCharacterNetworkMoveData : public FCharacterNetworkMoveData
{
private:
	using Super = FCharacterNetworkMoveData;

public:
	FGameplayTag RotationMode{AlsRotationModeTags::LookingDirection};

	FGameplayTag Stance{AlsStanceTags::Standing};

	FGameplayTag MaxAllowedGait{AlsGaitTags::Walking};

public:
	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& Move, ENetworkMoveType MoveType) override;

	virtual bool Serialize(UCharacterMovementComponent& Movement, FArchive& Archive, UPackageMap* Map, ENetworkMoveType MoveType) override;
};

/**
 * @brief 客户端移动网络数据容器
 */
class RPG005_API FAlsCharacterNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{
public:
	FAlsCharacterNetworkMoveData MoveData[3];

public:
	FAlsCharacterNetworkMoveDataContainer();
};

/**
 * @brief 客户端移动缓存数据
 */
class RPG005_API FAlsSavedMove : public FSavedMove_Character
{
private:
	using Super = FSavedMove_Character;

public:
	FGameplayTag RotationMode{AlsRotationModeTags::LookingDirection};

	FGameplayTag Stance{AlsStanceTags::Standing};

	FGameplayTag MaxAllowedGait{AlsGaitTags::Walking};

public:
	virtual void Clear() override;

	virtual void SetMoveFor(ACharacter* Character, float NewDeltaTime, const FVector& NewAcceleration,
	                        FNetworkPredictionData_Client_Character& PredictionData) override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const override;

	virtual void CombineWith(const FSavedMove_Character* PreviousMove, ACharacter* Character,
	                         APlayerController* PlayerController, const FVector& PreviousStartLocation) override;

	virtual void PrepMoveFor(ACharacter* Character) override;
};

/**
 * @brief 角色移动客户端预测数据
 */
class RPG005_API FAlsNetworkPredictionData : public FNetworkPredictionData_Client_Character
{
private:
	using Super = FNetworkPredictionData_Client_Character;

public:
	FAlsNetworkPredictionData(const UCharacterMovementComponent& Movement);

	virtual FSavedMovePtr AllocateNewMove() override;
};

/**
 * 角色基础移动组件
 */
UCLASS()
class RPG005_API UAlsCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend FAlsSavedMove;

private:
	FAlsCharacterNetworkMoveDataContainer MoveDataContainer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	TObjectPtr<UAlsMovementSettings> MovementSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FAlsMovementGaitSettings GaitSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FGameplayTag RotationMode{AlsRotationModeTags::LookingDirection};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FGameplayTag Stance{AlsStanceTags::Standing};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FGameplayTag MaxAllowedGait{AlsGaitTags::Walking};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	bool bMovementModeLocked;
	
	

	// Valid only on locally controlled characters.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FRotator PreviousControlRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient, Meta = (AllowPrivateAccess))
	FVector PendingPenetrationAdjustment;

public:
	FAlsPhysicsRotationDelegate OnPhysicsRotation;

	UPROPERTY(EditAnywhere,Category = "Setting")
	bool AllowPhysicsRotationDuringAnimRootMotion{false};
	

public:
	UAlsCharacterMovementComponent();

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* Property) const override;
#endif

	virtual void BeginPlay() override;

	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

public:
	virtual float GetMaxAcceleration() const override;

	virtual float GetMaxBrakingDeceleration() const override;

protected:
	virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaTime) override;

public:
	virtual void PhysicsRotation(float DeltaTime) override;

protected:
	virtual void PhysWalking(float DeltaTime, int32 Iterations) override;

	virtual void PhysNavWalking(float DeltaTime, int32 Iterations) override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	virtual void PerformMovement(float DeltaTime) override;

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
	virtual void SmoothClientPosition(float DeltaTime) override;

	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAcceleration) override;

	virtual void ComputeFloorDist(const FVector& CapsuleLocation, float LineDistance, float SweepDistance, FFindFloorResult& OutFloorResult,
	                              float SweepRadius, const FHitResult* DownwardSweepResult) const override;

private:
	void SavePenetrationAdjustment(const FHitResult& Hit);

	void ApplyPendingPenetrationAdjustment();

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character Movement")
	void SetMovementSettings(UAlsMovementSettings* NewMovementSettings);

	const FAlsMovementGaitSettings& GetGaitSettings() const;

private:
	void RefreshGaitSettings();

public:
	void SetRotationMode(const FGameplayTag& NewModeTag);

	void SetStance(const FGameplayTag& NewStanceTag);

	void SetMaxAllowedGait(const FGameplayTag& NewGaitTag);

private:
	void RefreshMaxWalkSpeed();

public:
	float CalculateGaitAmount() const;

	void SetMovementModeLocked(bool bNewMovementModeLocked);
};

inline const FAlsMovementGaitSettings& UAlsCharacterMovementComponent::GetGaitSettings() const
{
	return GaitSettings;
}
