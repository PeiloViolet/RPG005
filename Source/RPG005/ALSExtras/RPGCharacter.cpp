#include "RPGCharacter.h"
#include "CCMCameraFunctionLibrary.h"
#include "CCMPlayerCameraManager.h"
#include "RPG005/ALSCamera/AlsCameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Game/ACFFunctionLibrary.h"
#include "RPG_Controller.h"
#include "Components/ACFInteractionComponent.h"
#include "Components/ACFLocomotionComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Utility/GameplayTags/RPGGameplayTags.h"

ARPGCharacter::ARPGCharacter()
{
	Camera = CreateDefaultSubobject<UAlsCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh());
	Camera->SetRelativeRotation_Direct({0.0f, 90.0f, 0.0f});

	InteractComp = CreateDefaultSubobject<UACFInteractionComponent>(TEXT("InteractionComponent"));

	bReplicates = true;
	
	//StatisticsComp->OnStatisiticReachesZero.AddDynamic(this,&ARPGCharacter::OnStaminaReachesZero);
	
}

void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	RPG_Controller = Cast<ARPG_Controller>(GetController());
	
	if(RPG_Controller)
	{
		RPG_Controller->TargetingComponent->OnTargetingStateChanged.AddDynamic(this, &ARPGCharacter::OnTargetChangedSetRotationMode);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RPG_Controller is not valid"))
	}
}

void ARPGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Parameters;
	Parameters.bIsPushBased = true;

	Parameters.Condition = COND_SkipOwner;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, BaseLayerMode, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RPGLocomotionDirection, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RPGActionTransform, Parameters)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RPGTargetInfo, Parameters)
}

void ARPGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RefreshRPGTargetInfo();
	RefreshRPGActionTransformInfo(RPGActionTransform);
	
	TryRefreshRPGActionTransform(DeltaTime);
	TryRefreshRPGActionTransformWithLocked(DeltaTime);
	RPGCalculateDirection();
	RefresCrosshair(RotationMode);
}

void ARPGCharacter::NotifyControllerChanged()
{
	const auto* PreviousPlayer{Cast<APlayerController>(PreviousController)};
	if (IsValid(PreviousPlayer))
	{
		auto* EnhancedInputSubsystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PreviousPlayer->GetLocalPlayer())};
		if (IsValid(EnhancedInputSubsystem))
		{
			EnhancedInputSubsystem->RemoveMappingContext(InputMappingContext);
		}
	}

	auto* NewPlayer{Cast<APlayerController>(GetController())};
	if (IsValid(NewPlayer))
	{
		NewPlayer->InputYawScale_DEPRECATED = 1.0f;
		NewPlayer->InputPitchScale_DEPRECATED = 1.0f;
		NewPlayer->InputRollScale_DEPRECATED = 1.0f;

		auto* EnhancedInputSubsystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer->GetLocalPlayer())};
		if (IsValid(EnhancedInputSubsystem))
		{
			EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	Super::NotifyControllerChanged();
}

void ARPGCharacter::CalcCamera(const float DeltaTime, FMinimalViewInfo& ViewInfo)
{
	if (Camera->IsActive())
	{
		Camera->GetViewInfo(ViewInfo);
		return;
	}

	Super::CalcCamera(DeltaTime, ViewInfo);
}

void ARPGCharacter::SetBaseLayerMode(const FGameplayTag& NewModeTag)
{
	if (BaseLayerMode != NewModeTag)
	{
		const auto PreviousMode{BaseLayerMode};

		BaseLayerMode = NewModeTag;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, BaseLayerMode, this)

		OnBaseLayerModeChanged(PreviousMode);

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			ServerSetBaseLayerMode(NewModeTag);
		}
	}
}

void ARPGCharacter::ServerSetBaseLayerMode_Implementation(const FGameplayTag& NewModeTag)
{
	SetBaseLayerMode(NewModeTag);
}

void ARPGCharacter::OnReplicated_BaseLayerMode(const FGameplayTag& PreviousModeTag)
{
	OnBaseLayerModeChanged(PreviousModeTag);
}

void ARPGCharacter::OnBaseLayerModeChanged_Implementation(const FGameplayTag& PreviousModeTag) {}

/*void ARPGCharacter::OnStaminaReachesZero(FGameplayTag Stat)
{
	if (Stat == StatisticTags::Stamina)
	{
		StopSprint();
	}
}*/

void ARPGCharacter::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& Unused, float& VerticalLocation)
{
	if (Camera->IsActive())
	{
		Camera->DisplayDebug(Canvas, DebugDisplay, VerticalLocation);
	}
	Super::DisplayDebug(Canvas, DebugDisplay, Unused, VerticalLocation);
}
