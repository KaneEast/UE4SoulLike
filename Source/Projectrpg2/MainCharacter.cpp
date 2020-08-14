#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "RPGSaveGame.h"
#include "ItemStorage.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 350.f;			// �J��������v���C���[�܂ł̋���
	CameraBoom->bUsePawnControlRotation = true;		// controller����̑�����󂯂�	

	GetCapsuleComponent()->SetCapsuleSize(27.f, 88.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;	// �J��������]����ƃv���C���[���ړ����鎞�J���������Ɍ���

	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// don't rotate when the controller rotates
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.f, 1500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.1f;

	Health		= 300.f;
	MaxHealth	= 300.f;
	Stamina		= 120.f;
	MaxStamina	= 150.f;
	Coins		= 0;

	RunningSpeed	= 500.f;
	SprintingSpeed	= 800.f;

	bShiftKeyDown	= false;
	bLMBDown		= false;

	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;
	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;
	InterpSpeed		 = 15.f;

	bInterpToEnemy		= false;
	bHasCombatTarget	= false;
	bMovingForward		= false;
	bMovingRight		= false;
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	MainPlayerController = Cast<AMainPlayerController>(GetController());

	// kane: gameplay logic
	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	//if (Map != "SunTemple")
	{
		// kane comment out for unknown error  :   
		// LogStreaming: Warning: Failed to read file 'D:/UnrealProjects/rpg2Cleaned/Projectrpg2/Saved/SaveGames/TestSaveSlot.sav' error.
		//LoadGameNoSwitch(); 
		if (MainPlayerController)
		{
			MainPlayerController->GameModeOnly();
		}
	}
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
			}

			SetMovementStatus((bMovingForward || bMovingRight) ? EMovementStatus::EMS_Sprinting : EMovementStatus::EMS_Normal);
			if (MovementStatus == EMovementStatus::EMS_Sprinting)
			{
				Stamina -= DeltaStamina;
			}
		}
		else // Shift key up
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				Stamina = 0;
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				SetMovementStatus((bMovingForward || bMovingRight) ? EMovementStatus::EMS_Sprinting : EMovementStatus::EMS_Normal);
				if (MovementStatus == EMovementStatus::EMS_Sprinting)
				{
					Stamina -= DeltaStamina;
				}

			}
		}
		else // Shift key up
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else // Shift key up
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		;
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LockAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotaion = FMath::RInterpTo(GetActorRotation(), LockAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotaion);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump",	IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction("Jump",	IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("ESC",		IE_Pressed, this, &AMainCharacter::ESCDown);
	PlayerInputComponent->BindAction("Sprint",	IE_Pressed, this, &AMainCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint",	IE_Released, this, &AMainCharacter::ShiftKeyUp);
	PlayerInputComponent->BindAction("LMB",		IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB",		IE_Released, this, &AMainCharacter::LMBUp);

	PlayerInputComponent->BindAxis("MoveForward",	this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",		this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn",			this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp",		this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAxis("TurnRate",		this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate",	this, &AMainCharacter::LookUpAtRate);
}

void AMainCharacter::MoveForward(float value)
{
	bMovingForward = false;
	if (AllowInput(value))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
		bMovingForward = true;
	}
}

void AMainCharacter::MoveRight(float value)
{
	bMovingRight = false;
	if (AllowInput(value))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);
		bMovingRight = true;
	}
}



// Turn Camera
void AMainCharacter::Turn(float Value)
{
	//if (AllowInput(Value))
	{
		AddControllerYawInput(Value);
	}
}

void AMainCharacter::LookUp(float Value)
{
	//if (AllowInput(Value))
	{
		AddControllerPitchInput(Value);
	}
}

void AMainCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMainCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

// debug function
void AMainCharacter::ShowPickupLocations()
{
	for (auto Location : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 10.f, 0.5f);
	}
}

void AMainCharacter::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon) EquippedWeapon->Destroy();
	EquippedWeapon = WeaponToSet;
}

void AMainCharacter::LMBDown()
{
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	if (MainPlayerController && MainPlayerController->bPauseMenuOpen) return;

	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
	else if (EquippedWeapon)
	{
		Attack();
	}
}

void AMainCharacter::ESCDown()
{
	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

void AMainCharacter::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			int32 Section = FMath::RandRange(0, 1);
			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;

			default:
				;
			}
		}

		if (EquippedWeapon->SwingSound)
		{
			//UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
		}
	}
}

void AMainCharacter::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bLMBDown)
	{
		Attack();
	}
}

void AMainCharacter::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
	class AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0.f)
	{
		Die();
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy) Enemy->bHasValidTarget = false;
		}
	}

	//DecrementHealth(DamageAmount);
	return DamageAmount;
}

void AMainCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	// TODO: Show dead UI
	// Respawn player
	MainPlayerController->DisplayDeadUI();
}

void AMainCharacter::Jump()
{
	if (MainPlayerController && MainPlayerController->bPauseMenuOpen) return;

	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		Super::Jump();
	}
}

bool AMainCharacter::AllowInput(float Value)
{
	// 1
	// can't move while open pause menu
	return (Value != 0.0f) &&
		(!bAttacking) &&
		(MovementStatus != EMovementStatus::EMS_Dead) &&
		!MainPlayerController->bPauseMenuOpen;

	// 2
	// can move while open pause menu

	//return (Controller != nullptr) &&
	//	(Value != 0.0f) &&
	//	(!bAttacking) &&
	//	(MovementStatus != EMovementStatus::EMS_Dead);
}

void AMainCharacter::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation();
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

void AMainCharacter::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();
		CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

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

void AMainCharacter::SaveGame()
{
	URPGSaveGame* SaveObject = Cast<URPGSaveGame>(UGameplayStatics::CreateSaveGameObject(URPGSaveGame::StaticClass()));

	SaveObject->CharacterStats.Health		= Health;
	SaveObject->CharacterStats.MaxHealth	= MaxHealth;
	SaveObject->CharacterStats.Stamina		= Stamina;
	SaveObject->CharacterStats.MaxStamina	= MaxStamina;
	SaveObject->CharacterStats.Coins		= Coins;
	SaveObject->CharacterStats.Location		= GetActorLocation();
	SaveObject->CharacterStats.Rotation		= GetActorRotation();

	FString MapName = GetWorld()->GetMapName();
	UE_LOG(LogTemp, Warning, TEXT("MapName: %s"), *MapName);
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	UE_LOG(LogTemp, Warning, TEXT("MapName: %s"), *MapName);
	SaveObject->CharacterStats.LevelName = MapName;
	UE_LOG(LogTemp, Warning, TEXT("SaveObject->CharacterStats.LevelName: %s"), *SaveObject->CharacterStats.LevelName);
	if (EquippedWeapon)
	{
		SaveObject->CharacterStats.WeaponName = EquippedWeapon->Name;
		SaveObject->CharacterStats.bWeaponParticles = EquippedWeapon->bWeaponParticles;
	}

	UGameplayStatics::SaveGameToSlot(SaveObject, SaveObject->SaveSlotName, SaveObject->UserIndex);
}

void AMainCharacter::LoadGame(bool LoadPosition)
{
	URPGSaveGame* Load = Cast<URPGSaveGame>(UGameplayStatics::CreateSaveGameObject(URPGSaveGame::StaticClass()));
	URPGSaveGame* LoadObject = Cast<URPGSaveGame>(UGameplayStatics::LoadGameFromSlot(Load->SaveSlotName, Load->UserIndex));

	if (LoadObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Level: %s"), *LoadObject->CharacterStats.LevelName);
		if (LoadObject->CharacterStats.LevelName != "")
		{
			FName Map(*LoadObject->CharacterStats.LevelName);
			SwitchLevel(Map);
		}

		Health		= LoadObject->CharacterStats.Health;
		MaxHealth	= LoadObject->CharacterStats.MaxHealth;
		Stamina		= LoadObject->CharacterStats.Stamina;
		MaxStamina	= LoadObject->CharacterStats.MaxStamina;
		Coins		= LoadObject->CharacterStats.Coins;

		if (WeaponStorage)
		{
			AItemStorage* Container = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
			if (Container)
			{
				FString WeaponName = LoadObject->CharacterStats.WeaponName;
				if (Container->WeaponMap.Num() > 0)
				{
					if (Container->WeaponMap.Contains(WeaponName))
					{
						AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(Container->WeaponMap[WeaponName]);
						if (Weapon)
						{
							Weapon->bWeaponParticles = LoadObject->CharacterStats.bWeaponParticles;
							Weapon->Equip(this);

						}
					}
				}

			}
		}

		if (LoadPosition)
		{
			SetActorLocation(LoadObject->CharacterStats.Location);
			SetActorRotation(LoadObject->CharacterStats.Rotation);
		}

		SetMovementStatus(EMovementStatus::EMS_Normal);
		GetMesh()->bPauseAnims = false;
		GetMesh()->bNoSkeletonUpdate = false;

		if (LoadObject->CharacterStats.LevelName != TEXT(""))
		{
			FName LevelName(*LoadObject->CharacterStats.LevelName);
			SwitchLevel(LevelName);
		}
	}
}

void AMainCharacter::LoadGameNoSwitch()
{
	URPGSaveGame* Load = Cast<URPGSaveGame>(UGameplayStatics::CreateSaveGameObject(URPGSaveGame::StaticClass()));
	URPGSaveGame* LoadObject = Cast<URPGSaveGame>(UGameplayStatics::LoadGameFromSlot(Load->SaveSlotName, Load->UserIndex));

	if (LoadObject)
	{
		Health		= LoadObject->CharacterStats.Health;
		MaxHealth	= LoadObject->CharacterStats.MaxHealth;
		Stamina		= LoadObject->CharacterStats.Stamina;
		MaxStamina	= LoadObject->CharacterStats.MaxStamina;
		Coins		= LoadObject->CharacterStats.Coins;

		if (WeaponStorage)
		{
			AItemStorage* Container = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
			if (Container)
			{
				FString WeaponName = LoadObject->CharacterStats.WeaponName;
				if (Container->WeaponMap.Num() > 0)
				{
					if (Container->WeaponMap.Contains(WeaponName))
					{
						AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(Container->WeaponMap[WeaponName]);
						if (Weapon)
						{
							Weapon->bWeaponParticles = LoadObject->CharacterStats.bWeaponParticles;
							Weapon->Equip(this);
						}
					}
				}
			}// Container
		}// WeaponStorage
	}
}