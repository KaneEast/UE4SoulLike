#include "ItemStorage.h"

AItemStorage::AItemStorage()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItemStorage::BeginPlay()
{
	Super::BeginPlay();
}
