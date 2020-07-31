#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SavedData")
	float Health;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SavedData")
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float Stamina;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float MaxStamina;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	FVector Location;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	FRotator Rotation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	int32 Coins;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	FString WeaponName;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	bool bWeaponParticles;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	FString LevelName;
};

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus :uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMax")

};


UCLASS()
class PROJECTRPG2_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	/* カメラからプレイヤーまでの距離 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/* フォローカメラ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/* camera turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	/** Set movement status and running speed */
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;

	/** Pressed down to enable sprinting */
	FORCEINLINE void ShiftKeyDown() { bShiftKeyDown = true; }

	/** Released to stop sprinting */
	FORCEINLINE void ShiftKeyUp() { bShiftKeyDown = false; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	/**
	/* Player Stats
	/*
	/*
	*/

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float Health;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float Stamina;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	float MaxStamina;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SavedData")
	int32 Coins;

	void DecrementHealth(float Amount)
	{
		Health -= Amount;
		if (Health <= 0.f) Die();
	}

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount) { Coins += Amount; }

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount) { Health = std::min(MaxHealth, Health + Amount); }

	void Die();

	virtual void Jump() override;

	// override AActor::TakeDamage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float value);	// Called for forwards/backwards input
	void MoveRight(float value);	// Called for Right/Left input

	void Turn(float Value);/** Called for mouse input add yaw rotation */
	void LookUp(float Value);/** Called for mouse input add pitch rotation */
	bool bMovingForward;
	bool bMovingRight;
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	bool bLMBDown;
	void LMBDown();
	void LMBUp() { bLMBDown = false; }

	void ESCDown();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	TArray<FVector> PickupLocations;
	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Items)
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AItem* ActiveOverlappingItem;
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	void SetEquippedWeapon(AWeapon* WeaponToSet);
	UFUNCTION(BlueprintCallable)
	AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();


	// interping
	float InterpSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bInterpToEnemy;
	
	inline void SetInterpToEnemy(bool Interp) { bInterpToEnemy = Interp; }



	// kane: may not need for sourlike action rpg
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }
	
	FRotator GetLookAtRotationYaw(FVector Target);

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	bool AllowInput(float Value);

	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
		void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool LoadPosition);

	UFUNCTION(BlueprintCallable)
	void LoadGameNoSwitch();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SavedData")
		TSubclassOf<class AItemStorage> WeaponStorage;

	
};
