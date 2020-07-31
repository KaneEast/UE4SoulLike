#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

/**
 * Kane: Related to MainAnim_BP
 */
UCLASS()
class PROJECTRPG2_API UMainAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:

    virtual void NativeInitializeAnimation() override;

    UFUNCTION(BlueprintCallable, Category = AnimationProperties)
    void UpdateAnimationProperties();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
    float MovementSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
    bool bIsInAir;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
    class APawn* Pawn;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
    class AMainCharacter* Main;
};
