#include "RPGCharacter.h"
#include "CCMCameraFunctionLibrary.h"
#include "CCMPlayerCameraManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Game/ACFFunctionLibrary.h"
#include "RPG_Controller.h"
#include "UI/ACFHUD.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RPG005/ALS/AlsAnimationInstance.h"
#include "RPG005/ALS/AlsCharacterMovementComponent.h"


void ARPGCharacter::RPGCalculateDirection()
{
	if(LocomotionState.bHasInput)
	{
		const float DeltaYawAngle = (UKismetMathLibrary::NormalizedDeltaRotator(
			UKismetMathLibrary::Conv_VectorToRotator(AlsCharacterMovement->GetCurrentAcceleration()),GetControlRotation())).Yaw;
		if(UKismetMathLibrary::InRange_FloatFloat(DeltaYawAngle,-30.f,30.f,true,true))
		{
			RPGLocomotionDirection = ERPGLocomotionDirection::Forward;
			return;
		}
		if(UKismetMathLibrary::InRange_FloatFloat(DeltaYawAngle,30.f,60.f,true,true))
		{
			RPGLocomotionDirection = ERPGLocomotionDirection::Forward_Right;
			return;
		}
		if(UKismetMathLibrary::InRange_FloatFloat(DeltaYawAngle,60.f,120.f,true,true))
		{
			RPGLocomotionDirection = ERPGLocomotionDirection::Right;
			return;
		}
		if(UKismetMathLibrary::InRange_FloatFloat(DeltaYawAngle,120.f,150.f,true,true))
		{
			RPGLocomotionDirection = ERPGLocomotionDirection::Backward_Right;
			return;
		}
		if(UKismetMathLibrary::InRange_FloatFloat(DeltaYawAngle,150.f,180.f,true,true)
		|| UKismetMathLibrary::InRange_FloatFloat(DeltaYawAngle,-180.f,-150.f,true,true))
		{
			RPGLocomotionDirection = ERPGLocomotionDirection::Backward;
			return;
		}
		if(UKismetMathLibrary::InRange_FloatFloat(DeltaYawAngle,-150.f,-120.f,true,true))
		{
			RPGLocomotionDirection = ERPGLocomotionDirection::Backward_Left;
			return;
		}
		if(UKismetMathLibrary::InRange_FloatFloat(DeltaYawAngle,-120.f,-60.f,true,true))
		{
			RPGLocomotionDirection = ERPGLocomotionDirection::Left;
			return;
		}
		if(UKismetMathLibrary::InRange_FloatFloat(DeltaYawAngle,-60.f,-30.f,true,true))
		{
			RPGLocomotionDirection = ERPGLocomotionDirection::Forward_Left;
			return;
		}
		RPGLocomotionDirection = ERPGLocomotionDirection::Forward;
		return;
	}
	RPGLocomotionDirection = ERPGLocomotionDirection::None;
}

void ARPGCharacter::RefreshRPGTargetInfo()
{
	if(RPGTargetInfo.HasTarget)
	{
		if(RPG_Controller && RPG_Controller->TargetingComponent && RPG_Controller->TargetingComponent->GetCurrentTarget())
		{
			RPGTargetInfo.Location = RPG_Controller->TargetingComponent->GetCurrentTarget()->GetActorLocation();
			RPGTargetInfo.Distance = UKismetMathLibrary::VSize(GetActorLocation() - RPGTargetInfo.Location);
			RPGTargetInfo.X_Y_Distance =  UKismetMathLibrary::VSizeXY(GetActorLocation() - RPGTargetInfo.Location);
			RPGTargetInfo.Rotation = RPG_Controller->TargetingComponent->GetCurrentTarget()->GetActorRotation();
			RPGTargetInfo.LookAtRotation.Pitch = GetActorRotation().Pitch;
			RPGTargetInfo.LookAtRotation.Yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), RPGTargetInfo.Location).Yaw;
			RPGTargetInfo.LookAtRotation.Roll = GetActorRotation().Roll;
		}
	}
	if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerRefreshRPGTargetInfo(RPGTargetInfo);
	}
}

void ARPGCharacter::ServerRefreshRPGTargetInfo_Implementation(FRPGTargetInfo ServerRPGTargetInfo)
{
	RPGTargetInfo = ServerRPGTargetInfo;
}

void ARPGCharacter::RefreshRPGActionTransformInfo(FRPGActionTransform RPGActionTransforminfo)
{
	if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerRefreshRPGActionTransformInfo_Implementation(RPGActionTransforminfo);
	}
}

void ARPGCharacter::ServerRefreshRPGActionTransformInfo_Implementation(FRPGActionTransform ServerRPGActionTransforminfo)
{
	this->RPGActionTransform = ServerRPGActionTransforminfo;
}

void ARPGCharacter::TryRefreshRPGActionTransform(float DeltaTime)
{
	if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		RefreshRPGActionTransform(DeltaTime);
	}
	if(GetLocalRole() == ROLE_Authority)
	{
		RefreshRPGActionTransform(DeltaTime);
		ServarRefreshRPGActionTransform(DeltaTime);
	}
}

void ARPGCharacter::ServarRefreshRPGActionTransform_Implementation(float DeltaTime)
{
	NetMulticastRefreshRPGActionTransform(DeltaTime);
}

void ARPGCharacter::NetMulticastRefreshRPGActionTransform_Implementation(float DeltaTime)
{
	RefreshRPGActionTransform(DeltaTime);
}

void ARPGCharacter::TryRefreshRPGActionTransformWithLocked(float DeltaTime)
{
	if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		RefreshRPGActionTransformWithLocked(DeltaTime);
		
	}
	if(GetLocalRole() == ROLE_Authority)
	{
		RefreshRPGActionTransformWithLocked(DeltaTime);
		ServarRefreshRPGActionTransformWithLocked(DeltaTime);
	}
}

void ARPGCharacter::ServarRefreshRPGActionTransformWithLocked_Implementation(float DeltaTime)
{
	NetMulticastRefreshRPGActionTransformWithLocked(DeltaTime);
}

void ARPGCharacter::RefreshRPGActionTransform(float DeltaTime)
{
	if(RPGActionTransform.bIsRefreshRPGActionTransform)
	{
		auto TargetRotation{GetCharacterMovement()->UpdatedComponent->GetComponentRotation()};
		if(RPGActionTransform.bIsRefreshRPGActionRotation && RPGActionTransform.Curve)
		{
			if(ActionsComp->GetCurrentActionTag() == RPGActionTransform.CurrentAnimActionTag)
			{
				RPGActionTransform.CurrentTime += DeltaTime;
				RPGActionTransform.Alpha = RPGActionTransform.Curve->GetFloatValue(UKismetMathLibrary::
				MapRangeClamped(RPGActionTransform.CurrentTime, 0.f, RPGActionTransform.TimeSpent,0.f, 1.f ));
				TargetRotation = UKismetMathLibrary::RLerp(RPGActionTransform.StartRotation, RPGActionTransform.InputRotation, RPGActionTransform.Alpha, true);

				GetCharacterMovement()->MoveUpdatedComponent(FVector::ZeroVector, TargetRotation, false);
				if(RPGActionTransform.CurrentTime >= RPGActionTransform.TimeSpent)
				{
					RPGActionTransform.CurrentTime = 0.f;
					
					RPGActionTransform.bIsRefreshRPGActionRotation = false;
				}
			}
			else
			{
				RPGActionTransform.bIsRefreshRPGActionRotation = false;
			}
		}
	}
}


void ARPGCharacter::NetMulticastRefreshRPGActionTransformWithLocked_Implementation(float DeltaTime)
{
	RefreshRPGActionTransformWithLocked(DeltaTime);
}

void ARPGCharacter::RefreshRPGActionTransformWithLocked(float DeltaTime)
{
	if(RPGActionTransform.bIsRefreshRPGActionTransform)
	{
		if(RPGActionTransform.bIsRefreshRPGActionRotationWithLocked && RPGActionTransform.DistanceRotationCurve)
		{
			if(RPGTargetInfo.HasTarget)
			{
				if(ActionsComp->GetCurrentActionTag() == RPGActionTransform.CurrentAnimActionTag)
				{
					const float InterpSpeed = RPGActionTransform.DistanceRotationCurve->GetFloatValue(RPGTargetInfo.X_Y_Distance);
					const FRotator TargetRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(),RPGTargetInfo.LookAtRotation,DeltaTime,InterpSpeed);
					GetCharacterMovement()->MoveUpdatedComponent(FVector::ZeroVector, TargetRotation, false);
				}
				else
				{
					RPGActionTransform.bIsRefreshRPGActionRotationWithLocked = false;
				}
			}
			else
			{
				RPGActionTransform.bIsRefreshRPGActionRotationWithLocked = false;
			}
		}
		else
		{
			RPGActionTransform.bIsRefreshRPGActionRotationWithLocked = false;
		}
	}
}


void ARPGCharacter::RefresCrosshair(FGameplayTag NewRotationMode)
{
	const FName CameraAim = (TEXT("Aim"));
	AACFHUD* RPGHUD = UACFFunctionLibrary::GetACFHUD(GetWorld());

	if(PreviousRotationMode != NewRotationMode)
	{
		if(RotationMode== AlsRotationModeTags::Aiming)
		{
			UCCMCameraFunctionLibrary::GetLocalCinematicCameraManager(GetWorld())->TriggerCameraEvent(CameraAim);
			if(RPGHUD)
			{
				RPGHUD->EnableCrosshair(true);
			}
		}
		else
		{
			UCCMCameraFunctionLibrary::StopCameraEvent(GetWorld(),CameraAim);
			if(RPGHUD)
			{
				RPGHUD->EnableCrosshair(false);
			}
		}
	}
	PreviousRotationMode = NewRotationMode;
}

