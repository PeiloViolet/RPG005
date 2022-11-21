// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class RPG005_API GameApp : public FDefaultGameModuleImpl
{
public:
	virtual  void StartupModule() override;

	virtual void ShutdownModule() override;
};