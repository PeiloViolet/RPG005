// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPG005.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, RPG005, "RPG005" );

void GameApp::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

#if WITH_EDITOR
	FModuleManager::Get().LoadModule(TEXT("RPG005Editor"));
#endif
	
}

void GameApp::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}
