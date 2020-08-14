// Copyright Epic Games, Inc. All Rights Reserved.


#include "Projectrpg2GameModeBase.h"

void AProjectrpg2GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Version 0.0.1"));
	}
}
