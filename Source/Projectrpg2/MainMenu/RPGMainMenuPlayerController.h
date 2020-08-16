// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGMainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTRPG2_API ARPGMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void OpenLevel(const FName& LevelName);
};
