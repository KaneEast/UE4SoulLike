#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStorage.generated.h"

UCLASS()
class PROJECTRPG2_API AItemStorage : public AActor
{
	GENERATED_BODY()
	
public:
	AItemStorage();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TMap<FString, TSubclassOf<class AWeapon> > WeaponMap;
};
