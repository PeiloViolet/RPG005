#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttackInterface.generated.h"

UINTERFACE(Blueprintable)
class RPG005_API UAttackInterface : public UInterface
{
	GENERATED_BODY()
};

class RPG005_API IAttackInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Action)
	void OnInputActionTriggered();

};
