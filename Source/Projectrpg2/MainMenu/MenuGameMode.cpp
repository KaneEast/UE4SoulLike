// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameMode.h"
#include "MenuHUD.h"
#include "RPGMainMenuPlayerController.h"

AMenuGameMode::AMenuGameMode()
{
	PlayerControllerClass = ARPGMainMenuPlayerController::StaticClass();
	HUDClass = AMenuHUD::StaticClass();
}