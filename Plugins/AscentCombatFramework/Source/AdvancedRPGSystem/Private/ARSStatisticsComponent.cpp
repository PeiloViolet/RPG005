// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 

#include "ARSStatisticsComponent.h"
#include "ARSFunctionLibrary.h"
#include "ARSTypes.h"
#include "Net/UnrealNetwork.h"
#include <Curves/CurveFloat.h>
#include <Engine/World.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetSystemLibrary.h>
#include <TimerManager.h>

// Sets default values for this component's properties
UARSStatisticsComponent::UARSStatisticsComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked
    // every frame.  You can turn these features off to improve performance if you
    // don't need them.
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
    // ...
}

void UARSStatisticsComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UARSStatisticsComponent, AttributeSet);
    DOREPLIFETIME(UARSStatisticsComponent, CurrentExps);
    DOREPLIFETIME(UARSStatisticsComponent, ExpToNextLevel);
	DOREPLIFETIME(UARSStatisticsComponent, Perks);

}

void UARSStatisticsComponent::InitializeAttributeSet()
{
    if (GetOwner()->HasAuthority()) {
        InitilizeLevelData();

        Internal_InitializeStats();

        StartRegeneration();
    }
}

// Called when the game starts
void UARSStatisticsComponent::BeginPlay()
{

    Super::BeginPlay();

    if (bAutoInitialize) {
        InitializeAttributeSet();
    }
}

// Called every frame
void UARSStatisticsComponent::TickComponent(
    float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UARSStatisticsComponent::RegenerateStat()
{
    for (const auto& elem : AttributeSet.Statistics) {
        if (elem.HasRegeneration) {
            if (regenDelay.Contains(elem.StatType)  ) {
                FDateTime* before = regenDelay.Find(elem.StatType);
                if (before) {
                    const FTimespan delay = FDateTime::Now() - *before;
                    if (delay.GetSeconds() > elem.RegenDelay) {
                        regenDelay.Remove(elem.StatType);
                    } else if(elem.RegenValue > 0.f){
                        continue;
                    }
                }
            }
            FStatisticModifier modifier;
            modifier.Statistic = elem.StatType;
            modifier.Value = elem.RegenValue * RegenerationTimeInterval;
            Internal_ModifyStat(modifier, false);
        }
    }
}

void UARSStatisticsComponent::AddAttributeSetModifier_Implementation(
    const FAttributesSetModifier& attModifier)
{

    if (attModifier.Statistics.Num() == 0 && attModifier.Attributes.Num() == 0 && attModifier.Parameters.Num() == 0) {
        return;
    }

    if (!bIsInitialized) {
        storedUnactiveModifiers.Add(attModifier);
        return;
    }

    switch (attModifier.ModType) {
    case EModifierType::EAdditive:
        Internal_AddModifier(attModifier);
        break;
    case EModifierType::EPercentage:
        FAttributesSetModifier _newmod = CreateAdditiveAttributeSetModifireFromPercentage(attModifier);
        Internal_AddModifier(_newmod);
        break;
    }
}

bool UARSStatisticsComponent::AddAttributeSetModifier_Validate(
    const FAttributesSetModifier& attModifier)
{
    return true;
}

void UARSStatisticsComponent::Internal_AddModifier(const FAttributesSetModifier& attModifier)
{

    activeModifiers.AddUnique(attModifier);

    GenerateStats();
}

void UARSStatisticsComponent::GenerateStats()
{

    TArray<FStatistic> currentValuesCopy;
    for (const FStatistic stat : AttributeSet.Statistics) {
        currentValuesCopy.Add(stat);
    }

    CalcualtePrimaryStats();
    GenerateSecondaryStat();

    for (FStatistic& stat : AttributeSet.Statistics) {
        FStatistic* oldStat = currentValuesCopy.FindByKey(stat);
        if (oldStat)
            stat.CurrentValue = UARSFunctionLibrary::GetNewCurrentValueForNewMaxValue(
                oldStat->CurrentValue, oldStat->MaxValue, stat.MaxValue);
    }

    OnAttributeSetModified.Broadcast();
}

void UARSStatisticsComponent::Internal_ModifyStat(
    const FStatisticModifier& StatMod, bool bResetDelay)
{
    if (!bIsInitialized)
        return;

    FStatistic* stat = AttributeSet.Statistics.FindByKey(StatMod.Statistic);

    if (stat) {
        const float oldValue = stat->CurrentValue;
        stat->CurrentValue += StatMod.Value; // *GetConsumptionMultiplierByStatistic(stat->StatType);
        stat->CurrentValue = FMath::Clamp(stat->CurrentValue, 0.f, stat->MaxValue);
        if (bResetDelay && stat->HasRegeneration && stat->RegenDelay > 0.f) {
            regenDelay.Add(stat->StatType, FDateTime::Now());
        }
        OnAttributeSetModified.Broadcast();
        OnStatisticChanged.Broadcast(stat->StatType, oldValue, stat->CurrentValue);

        if (FMath::IsNearlyZero(stat->CurrentValue)) {
            OnStatisiticReachesZero.Broadcast(stat->StatType);
        }
    }
}

void UARSStatisticsComponent::CalcualtePrimaryStats()
{
    AttributeSet.Attributes = baseAttributeSet.Attributes;

    for (const FAttributesSetModifier& attModifier : activeModifiers) {
        for (const auto& att : attModifier.Attributes) {
            ensure(UARSFunctionLibrary::IsValidAttributeTag(att.AttributeType));
            if (UARSFunctionLibrary::IsValidAttributeTag(att.AttributeType)) {
                FAttribute* _originalatt = AttributeSet.Attributes.FindByKey(att);
                if (_originalatt) {
                    *(_originalatt) = *(_originalatt) + att;
                }
            }
        }
    }
}

void UARSStatisticsComponent::CalcualteSecondaryStats()
{
    for (const FAttributesSetModifier& attModifier : activeModifiers) {
        for (const auto& att : attModifier.Parameters) {
            ensure(UARSFunctionLibrary::IsValidParameterTag(att.AttributeType));
            if (UARSFunctionLibrary::IsValidParameterTag(att.AttributeType)) {
                FAttribute* _originalatt = AttributeSet.Parameters.FindByKey(att);
                if (_originalatt) {
                    *(_originalatt) = *(_originalatt) + att;
                } else {
                    AttributeSet.Parameters.Add(att);
                }
            }
        }

        for (const auto& att : attModifier.Statistics) {
            ensure(UARSFunctionLibrary::IsValidStatisticTag(att.StatType));
            if (UARSFunctionLibrary::IsValidStatisticTag(att.StatType)) {
                FStatistic* _originalatt = AttributeSet.Statistics.FindByKey(att);
                if (_originalatt) {
                    *(_originalatt) = *(_originalatt) + att;
                } else {
                    AttributeSet.Statistics.Add(att);
                }
            }
        }
    }
}

FAttributesSetModifier
UARSStatisticsComponent::CreateAdditiveAttributeSetModifireFromPercentage(
    const FAttributesSetModifier& attModifier)
{
    FAttributesSetModifier newatt;
    newatt.Guid = attModifier.Guid;

    newatt.ModType = EModifierType::EAdditive;

    for (const auto& att : attModifier.Attributes) {
        const FAttribute* _originalatt = AttributeSet.Attributes.FindByKey(att);
        if (_originalatt) {

            const float newval = _originalatt->Value * att.Value / 100.f;
            const FAttribute _new(att.AttributeType, newval);
            newatt.Attributes.AddUnique(_new);
        }
    }
    for (const auto& att : attModifier.Parameters) {
        const FAttribute* _originalatt = AttributeSet.Parameters.FindByKey(att);
        if (_originalatt) {

            const float newval = _originalatt->Value * att.Value / 100.f;
            const FAttribute _new(att.AttributeType, newval);
            newatt.Parameters.AddUnique(_new);
        }
    }
    for (const auto& att : attModifier.Statistics) {
        const FStatistic* originalatt = AttributeSet.Statistics.FindByKey(att);
        if (originalatt) {

            const float newval = originalatt->MaxValue * att.MaxValue / 100.f;
            const float newregenval = originalatt->RegenValue * att.RegenValue / 100.f;
            const float newRegenDelay = originalatt->RegenDelay * att.RegenDelay / 100.f;
            const FStatistic newAtt(att.StatType, newval, newregenval, newRegenDelay);
            newatt.Statistics.AddUnique(newAtt);
        }
    }
    return newatt;
}

void UARSStatisticsComponent::GenerateSecondaryStat()
{
    AttributeSet.Parameters = baseAttributeSet.Parameters;
    AttributeSet.Statistics = baseAttributeSet.Statistics;

    if (StatsLoadMethod != EStatsLoadMethod::EUseDefaultsWithoutGeneration) {
		GenerateSecondaryStatFromCurrentPrimaryStat();
    }
    CalcualteSecondaryStats();
}

void UARSStatisticsComponent::GenerateSecondaryStatFromCurrentPrimaryStat()
{

    for (const FAttribute& primaryatt : AttributeSet.Attributes) {
        FGenerationRule rules = UARSFunctionLibrary::GetGenerationRuleByPrimaryAttributeType(
            primaryatt.AttributeType);

        for (const FAttributeInfluence& att : rules.InfluencedParameters) {
            if (att.CurveValue) {
                FAttribute* targetAttribute = AttributeSet.Parameters.FindByKey(att.TargetParameter);
                if (targetAttribute) {
                    targetAttribute->Value += att.CurveValue->GetFloatValue(primaryatt.Value);
                } else {
                    const float param = att.CurveValue->GetFloatValue(primaryatt.Value);
                    const FAttribute localatt = FAttribute(att.TargetParameter, param);
                    AttributeSet.Parameters.AddUnique(localatt);
                }
            }
        }

        for (const FStatInfluence& stat : rules.InfluencedStatistics) {

            if (stat.CurveMaxValue) {
                FStatistic* targetStat = AttributeSet.Statistics.FindByKey(stat.TargetStat);
                if (targetStat) {
                    targetStat->MaxValue += stat.CurveMaxValue->GetFloatValue(primaryatt.Value);
                    targetStat->CurrentValue = targetStat->bStartFromZero ? 0.f : targetStat->MaxValue;
                } else {
                    const float param = stat.CurveMaxValue->GetFloatValue(primaryatt.Value);
                    const FStatistic localstat = FStatistic(stat.TargetStat, param, 0.f);
                    AttributeSet.Statistics.AddUnique(localstat);
                }
            }
            FStatistic* targetStat = AttributeSet.Statistics.FindByKey(stat.TargetStat);
            if (targetStat && stat.CurveRegenValue) {
                targetStat->RegenValue += stat.CurveRegenValue->GetFloatValue(primaryatt.Value);
                targetStat->HasRegeneration = targetStat->RegenValue != 0.f;
            }
        }
    }
}

void UARSStatisticsComponent::StartRegeneration_Implementation()
{
    if (!bIsRegenerationStarted && bCanRegenerateStatistics) {
        UWorld* world = GetWorld();
        if (world) {
            world->GetTimerManager().SetTimer(
                RegenTimer, this, &UARSStatisticsComponent::RegenerateStat,
                RegenerationTimeInterval, true);
            bIsRegenerationStarted = true;
        }
    }
}

bool UARSStatisticsComponent::StartRegeneration_Validate() { return true; }

void UARSStatisticsComponent::StopRegeneration_Implementation()
{
    if (bIsRegenerationStarted) {
        UWorld* world = GetWorld();
        // Calling MyUsefulFunction after 5 seconds without looping
        world->GetTimerManager().ClearTimer(RegenTimer);
        bIsRegenerationStarted = false;
    }
}

bool UARSStatisticsComponent::StopRegeneration_Validate() { return true; }

void UARSStatisticsComponent::AddExp_Implementation(int32 exp)
{

    if (LevelingType == ELevelingType::ECantLevelUp) {
        UE_LOG(LogTemp, Warning, TEXT("This Character can not level up! ARSStatisticsComponent"));
        return;
    }
    CurrentExps += exp;

    OnCurrentExpValueChanged.Broadcast(CurrentExps);
    if (CurrentExps >= ExpToNextLevel && CharacterLevel < UARSFunctionLibrary::GetMaxLevel()) {
        const int32 remainingExps = CurrentExps - ExpToNextLevel;
        CurrentExps = 0;
        CharacterLevel++;
        InitilizeLevelData();

        switch (LevelingType) {
        case ELevelingType::EGenerateNewStatsFromCurves:
            Internal_InitializeStats();
            break;
        case ELevelingType::EAssignPerksManually:
            Perks += PerksObtainedOnLevelUp;
            break;
        default:
            UE_LOG(LogTemp, Error, TEXT("A character that cannot level, just leved! ARSStatisticsComponent"));
            break;
        }
        OnLevelUp(CharacterLevel, remainingExps);
        AddExp(remainingExps);
    }
}

bool UARSStatisticsComponent::AddExp_Validate(int32 exp) { return true; }

void UARSStatisticsComponent::RemoveAttributeSetModifier_Implementation(
    const FAttributesSetModifier& attModifier)
{
    FAttributesSetModifier* localmod = activeModifiers.FindByKey(attModifier);
    if (localmod) {

        activeModifiers.RemoveSingle(*(localmod));

        GenerateStats();
    }
}

bool UARSStatisticsComponent::RemoveAttributeSetModifier_Validate(
    const FAttributesSetModifier& attModifier)
{
    return true;
}

void UARSStatisticsComponent::AddStatisticConsumptionMultiplier_Implementation(
    FGameplayTag statisticTag, float multiplier /*= 1.0f*/)
{
    if (UARSFunctionLibrary::IsValidStatisticTag(statisticTag)) {
        StatisticConsumptionMultiplier.Add(statisticTag, multiplier);
    }
}

bool UARSStatisticsComponent::AddStatisticConsumptionMultiplier_Validate(
    FGameplayTag statisticTag, float multiplier /*= 1.0f*/)
{
    return true;
}

float UARSStatisticsComponent::GetConsumptionMultiplierByStatistic(FGameplayTag statisticTag) const
{
    if (UARSFunctionLibrary::IsValidStatisticTag(statisticTag)) {
        const float* _mult = StatisticConsumptionMultiplier.Find(statisticTag);
        if (_mult) {
            return *(_mult);
        }
    }
    return 1.0f;
}

bool UARSStatisticsComponent::CheckCosts(
    const TArray<FStatisticModifier>& Costs) const
{
    for (const FStatisticModifier& cost : Costs) {
        if (!CheckCost(cost))
            return false;
    }
    return true;
}

bool UARSStatisticsComponent::CheckAttributesRequirements(
    const TArray<FAttribute>& Requirements) const
{
    for (const FAttribute& att : Requirements) {
        if (!UARSFunctionLibrary::IsValidAttributeTag(att.AttributeType)) {
            UE_LOG(LogTemp, Log,
                TEXT("Invalid Primary Attribute Tag!!! - "
                     "CheckPrimaryAttributeRequirements"));
            return false;
        }
        const FAttribute* localatt = AttributeSet.Attributes.FindByKey(att.AttributeType);
        if (localatt && localatt->Value < att.Value)
            return false;
    }
    return true;
}

bool UARSStatisticsComponent::CheckCost(const FStatisticModifier& Cost) const
{
    const FStatistic* stat = AttributeSet.Statistics.FindByKey(Cost.Statistic);
    if (stat) {
        return stat->CurrentValue > (Cost.Value * GetConsumptionMultiplierByStatistic(stat->StatType));
    } else {
        UE_LOG(LogTemp, Warning,
            TEXT("Missing Statistic! - ARSStatistic Component"));
    }
    return false;
}

void UARSStatisticsComponent::ConsumeStatistics(
    const TArray<FStatisticModifier>& Costs)
{
    for (const FStatisticModifier& cost : Costs) {
        FStatisticModifier modifier = cost;
        modifier.Value *= -1;
        ModifyStat(modifier);
    }
}

void UARSStatisticsComponent::OnRep_AttributeSet()
{
    OnAttributeSetModified.Broadcast();
}

void UARSStatisticsComponent::Internal_InitializeStats()
{
    bIsInitialized = false;

    AttributeSet.Statistics.Empty();
    AttributeSet.Attributes.Empty();
    AttributeSet.Parameters.Empty();

    TArray<FStatistic> currentValues;
    switch (StatsLoadMethod) {
    case EStatsLoadMethod::EUseDefaultsWithoutGeneration:
        baseAttributeSet = DefaultAttributeSet;
        AttributeSet = baseAttributeSet;
        break;
    case EStatsLoadMethod::EGenerateFromDefaultsPrimary:
        baseAttributeSet = DefaultAttributeSet;
        break;
    case EStatsLoadMethod::ELoadByLevel:
        baseAttributeSet.Attributes = GetPrimitiveAttributesForCurrentLevel();
        AttributeSet = baseAttributeSet;
        break;
    default:
        break;
    }

    if (StatsLoadMethod != EStatsLoadMethod::EUseDefaultsWithoutGeneration) {
        GenerateStats();
    }

    for (auto& statistic : AttributeSet.Statistics) {
        statistic.CurrentValue = statistic.bStartFromZero ? 0.f : statistic.MaxValue;
    }

    bIsInitialized = true;

    for (const FAttributesSetModifier& modifier : storedUnactiveModifiers) {
        AddAttributeSetModifier(modifier);
    }
    storedUnactiveModifiers.Empty();
}

void UARSStatisticsComponent::InitilizeLevelData()
{
    if (ExpForNextLevelCurve) {
        const float nextlevelexp = ExpForNextLevelCurve->GetFloatValue(CharacterLevel);
        ExpToNextLevel = FMath::TruncToInt(nextlevelexp);
    }
}

void UARSStatisticsComponent::OnLevelUp_Implementation(int32 newlevel,
    int32 _remainingExp)
{
    CharacterLevel = newlevel;

    OnCharacterLevelUp.Broadcast(CharacterLevel);
}

bool UARSStatisticsComponent::OnLevelUp_Validate(int32 newlevel,
    int32 _remainingExp)
{
    return true;
}

void UARSStatisticsComponent::ModifyStatistic(FGameplayTag Stat, float value)
{
    FStatisticModifier mod = FStatisticModifier(Stat, value);

    ModifyStat(mod);
}

float UARSStatisticsComponent::GetCurrentValueForStatitstic(
    FGameplayTag stat) const
{

    if (!UARSFunctionLibrary::IsValidStatisticTag(stat)) {
        UE_LOG(LogTemp, Warning,
            TEXT("INVALID STATISTIC TAG -  -  ARSStatistic Component"));
        return 0.f;
    }

    const FStatistic* intStat = AttributeSet.Statistics.FindByKey(stat);

    if (intStat) {
        return intStat->CurrentValue;
    }
    return 0.f;
}

float UARSStatisticsComponent::GetMaxValueForStatitstic(
    FGameplayTag stat) const
{
    if (!UARSFunctionLibrary::IsValidStatisticTag(stat)) {
        UE_LOG(LogTemp, Warning,
            TEXT("INVALID STATISTIC TAG -  -  ARSStatistic Component"));
        return 0.f;
    }

    const FStatistic* intStat = AttributeSet.Statistics.FindByKey(stat);

    if (intStat) {
        return intStat->MaxValue;
    }
    return 0.f;
}

float UARSStatisticsComponent::GetNormalizedValueForStatitstic(
    FGameplayTag stat) const
{
    const float max = GetMaxValueForStatitstic(stat);
    const float value = GetCurrentValueForStatitstic(stat);

    if (max != 0.f) {
        return value / max;
    }
    //     UE_LOG(LogTemp, Warning,
    //         TEXT("Missing Statistic! -  -  ARSStatistic Component"));
    return 0.f;
}

float UARSStatisticsComponent::GetAttributeValue(FGameplayTag stat) const
{
    if (!UARSFunctionLibrary::IsValidAttributeTag(stat)) {
        UE_LOG(LogTemp, Warning,
            TEXT("INVALID PRIMARY ATTRIBUTE TAG -  -  ARSStatistic Component"));
        return 0.f;
    }

    const FAttribute* intStat = AttributeSet.Attributes.FindByKey(stat);

    if (intStat) {
        return intStat->Value;
    }

    UE_LOG(LogTemp, Warning,
        TEXT("Missing  Primary Attribute '%s'! -  -  ARSStatistic Component"), *stat.GetTagName().ToString());

    return 0.f;
}

float UARSStatisticsComponent::GetParameterValue(FGameplayTag stat) const
{
    if (!UARSFunctionLibrary::IsValidParameterTag(stat)) {
        UE_LOG(
            LogTemp, Warning,
            TEXT("INVALID SECONDARY ATTRIBUTE TAG -  -  ARSStatistic Component"));
        return 0.f;
    }

    const FAttribute* intStat = AttributeSet.Parameters.FindByKey(stat);

    if (intStat) {
        return intStat->Value;
    }

    UE_LOG(LogTemp, Warning, TEXT("Missing  Secondary Attribute '%s! - ARSStatistic Component"), *stat.GetTagName().ToString());

    return 0.f;
}

FAttributesSet UARSStatisticsComponent::GetCurrentAttributeSet() const
{
    return AttributeSet;
}

int32 UARSStatisticsComponent::GetExpOnDeath() const
{
    if (!CanLevelUp())
        return ExpToGiveOnDeath;

    if (ExpToGiveOnDeathByCurrentLevel) {
        float expToGive = ExpToGiveOnDeathByCurrentLevel->GetFloatValue(CharacterLevel);
        return FMath::TruncToInt(expToGive);
    }

    UE_LOG(LogTemp, Error, TEXT("Invalid ExpToGiveOnDeathByCurrentLevel Curve!"));
    return -1;
}

void UARSStatisticsComponent::AssignPerkToAttribute_Implementation(const FGameplayTag& attributeTag, int32 numPerks /*= 1*/)
{
    if (numPerks > Perks) {
        UE_LOG(LogTemp, Warning, TEXT("You don't have enough perks!"));
        return;
    }

    PermanentlyModifyAttribute(attributeTag, numPerks);
    Perks -= numPerks;
}

bool UARSStatisticsComponent::AssignPerkToAttribute_Validate(const FGameplayTag& attributeTag, int32 numPerks /*= 1*/)
{
    return true;
}

void UARSStatisticsComponent::PermanentlyModifyAttribute_Implementation(
    const FGameplayTag& attribute, float deltaValue /*= 1.0f*/)
{
    const FAttribute* currValue = DefaultAttributeSet.Attributes.FindByKey(attribute);
    if (currValue) {
        FAttribute newValue(currValue->AttributeType,
            currValue->Value + deltaValue);
        DefaultAttributeSet.Attributes.Remove(newValue);
        DefaultAttributeSet.Attributes.AddUnique(newValue);
        InitializeAttributeSet();
    }
}

bool UARSStatisticsComponent::PermanentlyModifyAttribute_Validate(
    const FGameplayTag& attribute, float value /*= 1.0f*/)
{
    return true;
}

void UARSStatisticsComponent::ModifyStat_Implementation(
    const FStatisticModifier& StatMod)
{
    Internal_ModifyStat(StatMod);
}

bool UARSStatisticsComponent::ModifyStat_Validate(
    const FStatisticModifier& StatMod)
{
    return true;
}

TArray<FAttribute>
UARSStatisticsComponent::GetPrimitiveAttributesForCurrentLevel()
{
    return Internal_GetPrimitiveAttributesForCurrentLevel(
        AttributesByLevelCurves);
}

void UARSStatisticsComponent::OnComponentLoaded_Implementation()
{
    if (StatsLoadMethod != EStatsLoadMethod::EUseDefaultsWithoutGeneration) {
        GenerateStats();
    }
}

void UARSStatisticsComponent::OnComponentSaved_Implementation()
{
}

// TArray<FAttribute>
// UARSStatisticsComponent::GetPrimitiveAttributesForCurrentClassAndLevel()
// {
// 	if (AttributesByClass)
// 	{
// 		FAttributesByLevel* _fabl = AttributesByClass->FindRow<
// FAttributesByLevel>(ClassName, TEXT("Class OF The Character"));
//
// 		if (_fabl)
// 		{
// 			return
// Internal_GetPrimitiveAttributesForCurrentLevel(*_fabl);
// 		}
// 		else
// 		{
// 			UE_LOG(LogTemp, Log, TEXT("Remember to add Class and Levels
// Table!"));
// 		}
//
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Log, TEXT("Remember to add Class and Levels
// Table!"));
// 	}
//
// 	return TArray<FAttribute>();
// }

TArray<FAttribute>
UARSStatisticsComponent::Internal_GetPrimitiveAttributesForCurrentLevel(
    FAttributesByLevel attset)
{
    TArray<FAttribute> attributes;
    for (const auto& localatt : attset.PrimaryStatByLevelCurves) {

        if (localatt.Value) {
            FAttribute item;
            item.AttributeType = localatt.Key;
            item.Value = localatt.Value->GetFloatValue(CharacterLevel);
            attributes.AddUnique(item);
        }
    }
    return attributes;
}

void UARSStatisticsComponent::AddTimedAttributeSetModifier_Implementation(
    const FAttributesSetModifier& attModifier, float duration)
{
    if (duration == 0.f)
        return;

    if (!attModifier.Attributes.IsValidIndex(0) && !attModifier.Parameters.IsValidIndex(0) && !attModifier.Statistics.IsValidIndex(0)) {
        return;
    }

    Internal_AddModifier(attModifier);

    FTimerDelegate TimerDel;
    FTimerHandle TimerHandle;
    TimerDel.BindUFunction(this, FName("RemoveAttributeSetModifier"), attModifier);

    UWorld* world = GetWorld();
    if (world) {
        world->GetTimerManager().SetTimer(TimerHandle, TimerDel, duration, false);
    }
}

bool UARSStatisticsComponent::AddTimedAttributeSetModifier_Validate(
    const FAttributesSetModifier& attModifier, float duration)
{
    return true;
}

void UARSStatisticsComponent::ReinitializeAttributeSetFromNewDefault_Implementation(
    const FAttributesSet& newDefault)
{
    DefaultAttributeSet = newDefault;

    InitializeAttributeSet();
}

bool UARSStatisticsComponent::ReinitializeAttributeSetFromNewDefault_Validate(
    const FAttributesSet& newDefault)
{
    return true;
}

void UARSStatisticsComponent::SetNewLevelAndReinitialize_Implementation(uint8 newLevel)
{
    CharacterLevel = newLevel;

    InitializeAttributeSet();
}

bool UARSStatisticsComponent::SetNewLevelAndReinitialize_Validate(uint8 newLevel)
{
    return true;
}
