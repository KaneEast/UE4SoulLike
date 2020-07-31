#include "RPGSaveGame.h"

URPGSaveGame::URPGSaveGame()
{
	SaveSlotName = TEXT("TestSaveSlot");
	UserIndex = 0;

	CharacterStats.WeaponName = "";
	CharacterStats.LevelName = "";
}