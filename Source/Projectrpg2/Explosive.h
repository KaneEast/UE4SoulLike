#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Explosive.generated.h"

UCLASS()
class PROJECTRPG2_API AExplosive : public AItem
{
	GENERATED_BODY()
public:
	AExplosive();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float Damage;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;
};
