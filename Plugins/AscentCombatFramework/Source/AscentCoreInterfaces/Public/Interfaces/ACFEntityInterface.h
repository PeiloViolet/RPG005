// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved.

#pragma once

#include "ACFCoreTypes.h"
#include "CoreMinimal.h"
#include <UObject/Interface.h>
#include "ACFEntityInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UACFEntityInterface : public UInterface {
    GENERATED_BODY()
};

/**
 * 
 */
class ASCENTCOREINTERFACES_API IACFEntityInterface {
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    ETeam GetEntityCombatTeam();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    bool IsEntityAlive();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = ACF)
    void AssignTeamToEntity(ETeam inCombatTeam);
};
