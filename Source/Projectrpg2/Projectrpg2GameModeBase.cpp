// Copyright Epic Games, Inc. All Rights Reserved.


#include "Projectrpg2GameModeBase.h"
#include "MainPlayerController.h"
#include "MainMenu/MenuHUD.h"

AProjectrpg2GameModeBase::AProjectrpg2GameModeBase()
{
	//PlayerControllerClass = AMainPlayerController::StaticClass();
	HUDClass = AMenuHUD::StaticClass();
}

void AProjectrpg2GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Version 0.0.1"));
	}
}
