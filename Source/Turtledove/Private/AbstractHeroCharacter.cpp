// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractHeroCharacter.h"

AAbstractHeroCharacter::AAbstractHeroCharacter()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleRadius(16.0f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(32.0f);

	// Create a camera boom attached to the root (capsule)
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 180.0f;
	SpringArmComponent->SocketOffset = FVector(0.0f, 0.0f, 60.0f);
	SpringArmComponent->bAbsoluteRotation = true;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bAbsoluteRotation = true;

	// SpringArmComponent->bEnableCameraLag = true;
	// SpringArmComponent->CameraLagSpeed = 0.1f;
	// SpringArmComponent->CameraLagMaxDistance = 256.0f;

	// Create an orthographic camera (no perspective) and attach it to the boom
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	CameraComponent->OrthoWidth = 1024.0f;
	CameraComponent->OrthoNearClipPlane = 0.0f;
	CameraComponent->OrthoFarClipPlane = 1000.0f;

	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->bAutoActivate = true;

	// Configure character movement
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->Mass = 60.0f;
	MovementComponent->GravityScale = 1.0f;
	
	MovementComponent->AirControl = 1.0f;

	MovementComponent->JumpZVelocity = FMath::Sqrt(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 * MovementComponent->GetGravityZ);

	MovementComponent->GroundFriction = 1.0f;
	MovementComponent->MaxWalkSpeed = 250.0f;
	MovementComponent->MinAnalogWalkSpeed = 200.0f;
	MovementComponent->MaxWalkSpeedCrouched = 100.0f;
	MovementComponent->BrakingDecelerationWalking = 2500.0f;
	MovementComponent->MaxStepHeight = 0.0f;
	MovementComponent->SetWalkableFloorAngle(45.0f);

	MovementComponent->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	MovementComponent->bConstrainToPlane = true;
	MovementComponent->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	MovementComponent->bUseFlatBaseForFloorChecks = true;

	// 	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
	// 	TextComponent->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
	// 	TextComponent->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
	// 	TextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	// 	TextComponent->SetupAttachment(RootComponent);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;
}

void AAbstractHeroCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
	if (GetSprite()->GetFlipbook() != DesiredAnimation)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void AAbstractHeroCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCharacter();
	UpdateAnimation();
	UpdateCamera();
}

void AAbstractHeroCharacter::UpdateCharacter()
{
	const FVector PlayerVelocity = GetVelocity();
	float TravelDirection = PlayerVelocity.X;
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

void AAbstractHeroCharacter::UpdateCamera() 
{
	if (Controller != nullptr) 
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);

		int ScreenWidth = 0;
		int ScreenHeight = 0;
		PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);

		FVector2D ScreenLocation;
		PlayerController->ProjectWorldLocationToScreen(GetActorLocation(), ScreenLocation);
		UE_LOG(LogTemp, Warning, TEXT("Character's Screen Position is %f, %f"), ScreenLocation.X, ScreenLocation.Y);

		SpringArmComponent->CameraLagSpeed = FMath::Abs(ScreenWidth / 2 - ScreenLocation.X) / ScreenWidth / 2 * SpringArmComponent->CameraLagMaxDistance;
		UE_LOG(LogTemp, Warning, TEXT("Character's CameraLagSpeed is %f"), SpringArmComponent->CameraLagSpeed);
	}
}


void AAbstractHeroCharacter::MoveRight(float Value)
{
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

