#include "RPGTollsClass.h"
#include <Engine/World.h>
#include <Kismet/GameplayStatics.h>

void RPGTollsClass::OpenLevel(const FName& LevelName)
{
	if (!GEngine) return;

	if (UWorld* World = GEngine->GetWorld())
	{
		FString CurrentLevel = World->GetMapName();
		CurrentLevel.RemoveFromStart(World->StreamingLevelsPrefix);

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