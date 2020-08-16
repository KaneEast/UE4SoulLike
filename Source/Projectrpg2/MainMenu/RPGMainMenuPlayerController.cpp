// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGMainMenuPlayerController.h"
#include <Kismet/GameplayStatics.h>

// todo: have same code. needs refactoring
void ARPGMainMenuPlayerController::OpenLevel(const FName& LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();
		CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

		FName CurrentLevelName(*CurrentLevel);
		if (CurrentLevelName != LevelName)
		{
			FString Level = LevelName.ToString();
			UE_LOG(LogTemp, Warning, TEXT("CurrentLevel: %s"), *CurrentLevel)
				UE_LOG(LogTemp, Warning, TEXT("LevelName: %s"), *Level)
				UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}