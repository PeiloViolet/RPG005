// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "ACMCollisionManagerComponent.h"
#include <Components/MeshComponent.h>
#include <WorldCollision.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Engine/World.h>
#include <TimerManager.h>
#include <Templates/Function.h>
#include <Engine/EngineTypes.h>
#include "ACMCollisionsMasterComponent.h"
#include <GameFramework/GameMode.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/GameModeBase.h>
#include <Particles/ParticleSystemComponent.h>
#include <Sound/SoundBase.h>
#include <Components/SceneComponent.h>
#include <Sound/SoundCue.h>
#include "Components/ActorComponent.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UACMCollisionManagerComponent::UACMCollisionManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetComponentTickEnabled(false);
}

void UACMCollisionManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
	SetStarted(false);
}

void UACMCollisionManagerComponent::UpdateCollisions()
{
	if (damageMesh)
	{
		DisplayDebugTraces();

		if (pendingDelete.IsValidIndex(0)) {
			
			for (const auto toDelete : pendingDelete) {
				if (activatedTraces.Contains(toDelete)) {
					activatedTraces.Remove(toDelete);				
				}
				alreadyHitActors.Remove(toDelete);
			}
			pendingDelete.Empty();
		}
		if (activatedTraces.Num() == 0)
		{
			SetStarted(false);
			return;

		}
		if ( CollisionChannels.IsValidIndex(0))
		{
			for (const TPair<FName, FTraceInfo>& currentTrace : activatedTraces)
			{
				if (damageMesh->DoesSocketExist(currentTrace.Value.StartSocket) &&
					damageMesh->DoesSocketExist(currentTrace.Value.EndSocket))
				{
					FHitResult hitRes;

					const FVector StartPos = damageMesh->GetSocketLocation(currentTrace.Value.StartSocket);
					const FVector EndPos = damageMesh->GetSocketLocation(currentTrace.Value.EndSocket);
					const FRotator orientation = GetLineRotation(StartPos, EndPos);
					FCollisionQueryParams Params;


					if (IgnoredActors.Num() > 0) {
						Params.AddIgnoredActors(IgnoredActors);
					}


					if (bIgnoreOwner) {
						Params.AddIgnoredActor(GetActorOwner());
					}


					if (!bAllowMultipleHitsPerSwing)
					{
						FHitActors* hitResact = alreadyHitActors.Find(currentTrace.Key);
						if (hitResact && hitResact->AlreadyHitActors.Num() > 0)
						{
							Params.AddIgnoredActors(hitResact->AlreadyHitActors);
						}
					}

					Params.bReturnPhysicalMaterial = true;
					Params.bTraceComplex = true;

					FCollisionObjectQueryParams ObjectParams;
					for (ECollisionChannel channel : CollisionChannels) {
						ObjectParams.AddObjectTypesToQuery(channel);
					}


					if (ObjectParams.IsValid() == false)
					{
						UE_LOG(LogTemp, Warning, TEXT("Invalid Collision Channel"));
						return;
					}

					UWorld* world = GetWorld();
					if (world)
					{

						const bool bHit = world->SweepSingleByObjectType(hitRes, StartPos, EndPos, orientation.Quaternion(), ObjectParams,
							FCollisionShape::MakeSphere(currentTrace.Value.Radius), Params);

						if (bHit)
						{
							OnCollisionDetected.Broadcast(currentTrace.Key, hitRes);
							if (!bAllowMultipleHitsPerSwing)
							{
								FHitActors* hitResact = alreadyHitActors.Find(currentTrace.Key);
								if (hitResact)
								{
									hitResact->AlreadyHitActors.Add(hitRes.GetActor());

								}
								else
								{
									FHitActors newHit;
									newHit.AlreadyHitActors.Add(hitRes.GetActor());
									alreadyHitActors.Add(currentTrace.Key, newHit);
								}
							}
							if (bAutoApplyDamage)
							{
								ApplyDamage(hitRes, currentTrace.Value);
							}
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Invalid Socket Names!!"));
				}
			}
		}
		else {
			SetStarted(false);
		}	
	}

}



void UACMCollisionManagerComponent::SetStarted(bool inStarted)
{
	bIsStarted = inStarted;
	AGameModeBase* gameMode = UGameplayStatics::GetGameMode(this);
	if (gameMode) {
		UACMCollisionsMasterComponent* collisionMaster = gameMode->FindComponentByClass< UACMCollisionsMasterComponent>();
		if (collisionMaster) {
			if (ShowDebugInfo == EDebugType::EAlwaysShowDebug || bIsStarted) {
				collisionMaster->AddComponent(this);
			}
			else {
				collisionMaster->RemoveComponent(this);
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Add Collisions Master o your Game Mode!"));
		}
	}
}

FRotator UACMCollisionManagerComponent::GetLineRotation(FVector start, FVector end)
{
	FVector _diff = end - start;
	return _diff.Rotation();
}

void UACMCollisionManagerComponent::SetupCollisionManager(class UMeshComponent* inDamageMesh)
{
	damageMesh = inDamageMesh;

	if (!damageMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Damage mesh!!"));
		return;
	}

	for (const auto& trace : DamageTraces) {
		UParticleSystemComponent* ParticleSystemComp = NewObject< UParticleSystemComponent>(this, UParticleSystemComponent::StaticClass());
		ParticleSystemComp->SetupAttachment(damageMesh);
		ParticleSystemComp->SetRelativeLocation(FVector::ZeroVector);
		ParticleSystemComponents.Add(trace.Key, ParticleSystemComp);
		ParticleSystemComp->RegisterComponent();
	}
}

void UACMCollisionManagerComponent::AddActorToIgnore(class AActor* ignoredActor)
{
	IgnoredActors.AddUnique(ignoredActor);
}

void UACMCollisionManagerComponent::AddCollisionChannel( TEnumAsByte<ECollisionChannel> inTraceChannel)
{
	CollisionChannels.AddUnique(inTraceChannel);
}

void UACMCollisionManagerComponent::SetCollisionChannels( TArray<TEnumAsByte<ECollisionChannel>> inTraceChannels)
{
	ClearCollisionChannels();

	for (const TEnumAsByte<ECollisionChannel>& chan : inTraceChannels)
	{
		AddCollisionChannel(chan);
	}
}

void UACMCollisionManagerComponent::ClearCollisionChannels()
{
	CollisionChannels.Empty();
}

void UACMCollisionManagerComponent::PerformLineTraceShot_Implementation(const FVector& direction)
{
	if (damageMesh && actorOwner) {

		const FVector start = damageMesh->GetSocketLocation(LineTraceInfo.StartSocket);
		const FVector end = start + (direction * LineTraceInfo.LineTraceLength);

		EDrawDebugTrace::Type debugType;
		switch (ShowDebugInfo)
		{
		case EDebugType::EAlwaysShowDebug:
		case EDebugType::EShowInfoDuringSwing:
			DrawDebugLine(GetWorld(), start, end, FColor::Red,true, 5.f, 1, 1.f);
			break;
		case EDebugType::EDontShowDebugInfos:
			debugType = EDrawDebugTrace::Type::None;
			break;
		default:
			debugType = EDrawDebugTrace::Type::None;
			break;
		}

		FCollisionQueryParams Params;
		if (IgnoredActors.Num() > 0) {
			Params.AddIgnoredActors(IgnoredActors);
		}

		if (bIgnoreOwner) {
			Params.AddIgnoredActor(GetActorOwner());
		}

		Params.bReturnPhysicalMaterial = true;
		Params.bTraceComplex = true;
		FCollisionObjectQueryParams ObjectParams;
		for (const ECollisionChannel& channel : CollisionChannels) {
			ObjectParams.AddObjectTypesToQuery(channel);
		}

		FHitResult outResult;	

		if (GetWorld()->LineTraceSingleByObjectType(outResult, start, end, ObjectParams, Params)) {
			ApplyPointDamage(outResult, LineTraceInfo);
		}
	}	
}

bool UACMCollisionManagerComponent::PerformLineTraceShot_Validate(const FVector& direction)
{
	return true;
}

void UACMCollisionManagerComponent::StartAllTraces_Implementation()
{
	activatedTraces.Empty();
	pendingDelete.Empty();

	for (const auto& damage : DamageTraces) {
		StartSingleTrace(damage.Key);
	}
}

bool UACMCollisionManagerComponent::StartAllTraces_Validate()
{
	return true;
}

void UACMCollisionManagerComponent::StopAllTraces_Implementation()
{	
	pendingDelete.Empty();
	for (const auto& trace : activatedTraces) {
		StopSingleTrace(trace.Key);
	}
}

bool UACMCollisionManagerComponent::StopAllTraces_Validate()
{
	return true;
}



void UACMCollisionManagerComponent::StartSingleTrace_Implementation(const FName& Name)
{
	FTraceInfo* _trace = DamageTraces.Find(Name);
	if (_trace)
	{
		if (pendingDelete.Contains(Name)) {
			pendingDelete.Remove(Name);
		}
		activatedTraces.Add(Name, *_trace);
		PlayTrails(Name);
		SetStarted(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Trace Name!!"));
	}
}

bool UACMCollisionManagerComponent::StartSingleTrace_Validate(const FName& Name) {
	return true;
}

void UACMCollisionManagerComponent::StopSingleTrace_Implementation(const FName& Name)
{
	if (activatedTraces.Contains(Name)) {
		StopTrails(Name);
		pendingDelete.AddUnique(Name);

		FHitActors* alreadyHit = alreadyHitActors.Find(Name);
		if (alreadyHit) {
			alreadyHit->AlreadyHitActors.Empty();
		}
	}
}

bool UACMCollisionManagerComponent::StopSingleTrace_Validate(const FName& Name)
{
	return true;
}

void  UACMCollisionManagerComponent::DisplayDebugTraces()
{
	TMap<FName, FTraceInfo> _sphere;

	FLinearColor DebugColor;
	switch (ShowDebugInfo)
	{
	case EDebugType::EAlwaysShowDebug:
		_sphere = DamageTraces;
		if (bIsStarted)
			DebugColor = DebugActiveColor;
		else
			DebugColor = DebugInactiveColor;
		break;
	case EDebugType::EShowInfoDuringSwing:
		if (bIsStarted)
		{
			_sphere = activatedTraces;

			DebugColor = DebugActiveColor;
		}	
		else
			return;
		break;
	case EDebugType::EDontShowDebugInfos:
		return;
	default:
		return;
	}

	UWorld* world = GetWorld();
	for (const TPair<FName, FTraceInfo>& box : _sphere)
	{
		if (damageMesh->DoesSocketExist(box.Value.StartSocket) &&
			damageMesh->DoesSocketExist(box.Value.EndSocket))
		{
			FHitResult hitRes;

			FVector StartPos = damageMesh->GetSocketLocation(box.Value.StartSocket);
			FVector EndPos = damageMesh->GetSocketLocation(box.Value.EndSocket);
	
			if (world)
			{
				UKismetSystemLibrary::SphereTraceSingle(this, StartPos, EndPos, box.Value.Radius, ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors,
					EDrawDebugTrace::ForOneFrame, hitRes, true, DebugColor);
			}
		}
	}

}

void UACMCollisionManagerComponent::StartTimedSingleTrace_Implementation(const FName& TraceName, float Duration)
{	
	UWorld* world = GetWorld();
	if (world )
	{
		StartSingleTrace(TraceName);
		FTimerHandle timerHandle;
		FTimerDelegate TimerDelegate;

		TFunction<void(void)> lambdaDelegate = [this, TraceName]() { HandleTimedSingleTraceFinished(TraceName); };
		TimerDelegate.BindLambda(lambdaDelegate);
		TraceTimers.Add(TraceName, timerHandle);
		world->GetTimerManager().SetTimer(timerHandle, TimerDelegate, Duration, false);
	}
}

bool UACMCollisionManagerComponent::StartTimedSingleTrace_Validate(const FName& TraceName, float Duration) {
	return true;
}


void UACMCollisionManagerComponent::StartAllTimedTraces_Implementation(float Duration)
{
	UWorld* world = GetWorld();
	if (world && !bAllTimedTraceStarted)
	{
		StartAllTraces();
		world->GetTimerManager().SetTimer(AllTraceTimer, this, &UACMCollisionManagerComponent::HandleAllTimedTraceFinished, Duration, false);
		bAllTimedTraceStarted = true;
	}
}

bool UACMCollisionManagerComponent::StartAllTimedTraces_Validate(float Duration) {
	return true;
}


void UACMCollisionManagerComponent::SetTraceConfig(const FName& traceName, const FTraceInfo& traceInfo)
{
	DamageTraces.Add(traceName, traceInfo);
}

void UACMCollisionManagerComponent::HandleTimedSingleTraceFinished(const FName& traceEnded)
{	
	StopSingleTrace(traceEnded);
	if (GetOwner()) {
		UWorld* world = GetWorld();
		if (world && TraceTimers.Contains(traceEnded))
		{
			FTimerHandle* handle = TraceTimers.Find(traceEnded);
			world->GetTimerManager().ClearTimer(*handle);
		}
	}
}

void UACMCollisionManagerComponent::HandleAllTimedTraceFinished()
{
	StopAllTraces();
	if (GetOwner()) {
		UWorld* world = GetWorld();
		if (world && bAllTimedTraceStarted)	{
			world->GetTimerManager().ClearTimer(AllTraceTimer);
			bAllTimedTraceStarted = false;
		}
	}
}



void UACMCollisionManagerComponent::ApplyDamage(const FHitResult& HitResult, const FTraceInfo& currentTrace)
{
	switch (currentTrace.DamageType)
	{
	case EDamageType::EPoint:
		ApplyPointDamage(HitResult, currentTrace);
		break;
	case EDamageType::EArea:
		ApplyAreaDamage(HitResult, currentTrace);
		break;
	default:
		ApplyPointDamage(HitResult, currentTrace);
		break;
	}
}

void UACMCollisionManagerComponent::ApplyPointDamage(const FHitResult& HitResult, const FBaseTraceInfo& currentTrace)
{
	if (HitResult.GetActor()->IsValidLowLevel())
	{
		const FVector damagerRelativePos = GetOwner()->GetActorLocation()
			- HitResult.GetActor()->GetActorLocation();
		const float damage = currentTrace.BaseDamage;
		FPointDamageEvent damageInfo;

		damageInfo.DamageTypeClass = currentTrace.DamageTypeClass;
		damageInfo.Damage = currentTrace.BaseDamage;
		damageInfo.HitInfo = HitResult;
		damageInfo.ShotDirection = damagerRelativePos;
		HitResult.GetActor()->TakeDamage(damage, damageInfo, GetActorOwner()->GetInstigatorController(), GetActorOwner());

		OnActorDamaged.Broadcast(HitResult.GetActor());
	}
}

void UACMCollisionManagerComponent::ApplyAreaDamage(const FHitResult& HitResult, const FTraceInfo& currentTrace)
{
	if (HitResult.GetActor()->IsValidLowLevel())
	{
		float damage = currentTrace.BaseDamage;
		FRadialDamageEvent damageInfo;

		damageInfo.DamageTypeClass = currentTrace.DamageTypeClass;
		damageInfo.Params.BaseDamage = currentTrace.BaseDamage;
		damageInfo.ComponentHits.Add( HitResult);
		damageInfo.Origin = HitResult.ImpactPoint;

		HitResult.GetActor()->TakeDamage(damage, damageInfo, GetActorOwner()->GetInstigatorController(), GetActorOwner());
		OnActorDamaged.Broadcast(HitResult.GetActor());
	}
}

void UACMCollisionManagerComponent::PlayTrails_Implementation(const FName& trail)
{
	if (!DamageTraces.Contains(trail) || !damageMesh) {
		return ;
	}
	FTraceInfo traceInfo = *DamageTraces.Find(trail);
	
	if (traceInfo.AttackParticle &&
		ParticleSystemComponents.Contains(trail) &&
		damageMesh->DoesSocketExist(traceInfo.StartSocket) &&
		damageMesh->DoesSocketExist(traceInfo.EndSocket)) {

		UParticleSystemComponent* partComp = *ParticleSystemComponents.Find(trail);
		partComp->SetTemplate(traceInfo.AttackParticle);
		partComp->BeginTrails(traceInfo.StartSocket, traceInfo.EndSocket, ETrailWidthMode::ETrailWidthMode_FromCentre, traceInfo.TrailLength);
	}

	if (traceInfo.AttackSound)	{
		UGameplayStatics::SpawnSoundAttached(traceInfo.AttackSound, damageMesh, traceInfo.StartSocket);
	}
}

bool UACMCollisionManagerComponent::PlayTrails_Validate(const FName& trail)
{
	return true;
}

void UACMCollisionManagerComponent::StopTrails_Implementation(const FName& trail)
{
	if (ParticleSystemComponents.Contains(trail) )
	{
		UParticleSystemComponent* partComp = *ParticleSystemComponents.Find(trail);
		partComp->EndTrails();
	}
}

bool UACMCollisionManagerComponent::StopTrails_Validate(const FName& trail)
{
	return true;
}

