#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Pickup.generated.h"

UCLASS()
class PROJECTRPG2_API APickup : public AItem
{
	GENERATED_BODY()
public:

	APickup();

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnPickupBP(class AMainCharacter* Target);


};
