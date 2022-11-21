#pragma once

#include "ATSTargetableInterface.h"
#include "RPG005/ALS/AlsCharacter.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "RPG005/ALS/Utility/RPGLocomotionDirection.h"
#include "Actors/ACFCharacter.h"
#include "RPG_Controller.h"
#include "Components/ACFInteractionComponent.h"
#include "Utility/RPGActionTransform.h"
#include "Utility/RPGTargetInfo.h"
#include "RPGCharacter.generated.h"

class UAlsCameraComponent;
class UAlsCharacterMovementComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class RPG005_API ARPGCharacter : public AAlsCharacter, public IATSTargetableInterface
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UAlsCameraComponent> Camera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<ARPG_Controller> RPG_Controller;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Als Character|Desired State", ReplicatedUsing = "OnReplicated_BaseLayerMode", Meta = (AllowPrivateAccess))
	FGameplayTag BaseLayerMode{AlsBaseLayerModeTags::AlsBaseLayer};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|Als Character", Transient, Meta = (AllowPrivateAccess))
	FGameplayTag PreviousRotationMode{RotationMode};

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UACFInteractionComponent> InteractComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> LookMouseAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> WalkAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> AlsCrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> RagdollAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> RollAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> RotationModeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> ViewModeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> SwitchShoulderAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> InteractAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> TargetAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> EquipAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> LightAttackAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input", Meta = (AllowPrivateAccess))
	TObjectPtr<UInputAction> HeavyAttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input",
		Meta = (AllowPrivateAccess, ClampMin = 0, ForceUnits = "x"))
	float LookUpMouseSensitivity{3.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input",
		Meta = (AllowPrivateAccess, ClampMin = 0, ForceUnits = "x"))
	float LookRightMouseSensitivity{3.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input",
		Meta = (AllowPrivateAccess, ClampMin = 0, ForceUnits = "deg/s"))
	float LookUpRate{90.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Als Character Input",
		Meta = (AllowPrivateAccess, ClampMin = 0, ForceUnits = "deg/s"))
	float LookRightRate{240.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|RPG Character")
	bool IsChargeAtack{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPG)
	bool CanSkipCharge{false};
	
	//关于蒙太奇过程中的转向设置
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|RPGCharacter")
	FRPGActionTransform RPGActionTransform;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|RPGCharacter")
	FRPGTargetInfo RPGTargetInfo;

	UPROPERTY(Replicated, BlueprintReadOnly)
	ERPGLocomotionDirection RPGLocomotionDirection;

	

public:
	ARPGCharacter();

	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged() override;
	
	UFUNCTION(BlueprintCallable, Category = "ALS|Als Character", Meta = (AutoCreateRefTerm = "NewModeTag"))
	void SetBaseLayerMode(const FGameplayTag& NewModeTag);
	
	UFUNCTION(Server, Reliable)
	void ServerSetBaseLayerMode(const FGameplayTag& NewModeTag);

	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	FGameplayTag SelectDodgeActionTag();
	
	UFUNCTION(BlueprintNativeEvent, Category = "ALS|Als Character")
	void OnBaseLayerModeChanged(const FGameplayTag& PreviousModeTag);


	
private:

	UFUNCTION()
	void OnReplicated_BaseLayerMode(const FGameplayTag& PreviousModeTag);
	
	UFUNCTION(BlueprintCallable)
	void RPGCalculateDirection();
	
	UFUNCTION(BlueprintCallable)
	void RefresCrosshair(FGameplayTag NewRotationMode);

protected:
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo) override;

	// Input

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* Input) override;

	UFUNCTION()
	void OnTargetChangedSetRotationMode(bool bIsTargeting);

//private:
public:
	void InputLookMouse(const FInputActionValue& ActionValue);

	void InputLook(const FInputActionValue& ActionValue);

	void InputMove(const FInputActionValue& ActionValue);
	
	UFUNCTION(BlueprintNativeEvent,Category = "Input")
	void InputSprint(const FInputActionValue& ActionValue);
	void InputSprint_Implementation(const FInputActionValue& ActionValue);
	
	void InputWalk();

	void InputCrouch();

	void InputJump(const FInputActionValue& ActionValue);

	void InputAim(const FInputActionValue& ActionValue);

	void InputRagdoll();

	void InputRoll();

	void InputDodge();

	void InputRotationMode();

	void InputViewMode();

	void InputSwitchShoulder();

	void InputInteract();

	void InputTarget();

	void InputEquip();
	
	void InputLightAttack();

	void InputHeavyAttack_Started(const FInputActionValue& ActionValue);
	void InputHeavyAttack_Ongoing(const FInputActionValue& ActionValue);
	void InputHeavyAttack_Triggered(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable,Category = "Input")
	void StartSprint();
	
	UFUNCTION(BlueprintCallable,Category = "Input")
	void StopSprint();
	//void OnStaminaReachesZero(FGameplayTag Stat);
	
	UFUNCTION(BlueprintCallable)
	void StartDodging();
	
	UFUNCTION()
	void RefreshRPGActionTransformInfo(FRPGActionTransform RPGActionTransforminfo);

	UFUNCTION(Server, Reliable)
	void ServerRefreshRPGActionTransformInfo(FRPGActionTransform ServerRPGActionTransforminfo);
	
	UFUNCTION()
	void RefreshRPGTargetInfo();

	UFUNCTION(Server, Reliable)
	void ServerRefreshRPGTargetInfo(FRPGTargetInfo ServerRPGTargetInfo);
	
	UFUNCTION(BlueprintCallable)
	void RefreshRPGActionTransform(float DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	void RefreshRPGActionTransformWithLocked(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void TryRefreshRPGActionTransformWithLocked(float DeltaTime);
	
	UFUNCTION(Server, Reliable)
	void ServarRefreshRPGActionTransformWithLocked(float DeltaTime);
	UFUNCTION(NetMulticast,Unreliable)
	void NetMulticastRefreshRPGActionTransformWithLocked(float DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	void TryRefreshRPGActionTransform(float DeltaTime);
	
	UFUNCTION(Server, Reliable)
	void ServarRefreshRPGActionTransform(float DeltaTime);
	UFUNCTION(NetMulticast,Unreliable)
	void NetMulticastRefreshRPGActionTransform(float DeltaTime);
	
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ATS)
	//void OnTargetAcquired(UATSTargetPointComponent* point);
	virtual void OnTargetAcquired_Implementation(UATSTargetPointComponent* point) override;
	
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ATS)
	//void OnTargetLosed();
	virtual void OnTargetLosed_Implementation() override;

	UFUNCTION(BlueprintNativeEvent)
	void GetsTargeted(UATSTargetPointComponent* point);
	void GetsTargeted_Implementation(UATSTargetPointComponent* point);

	UFUNCTION(BlueprintNativeEvent)
	void LoseTargeted();
	void LoseTargeted_Implementation();
	// Debug

public:
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& Unused, float& VerticalLocation) override;
};
