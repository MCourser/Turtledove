// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"

#include "PaperFlipbookComponent.h"

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "AbstractHeroCharacter.generated.h"

/**
 *
 */
UCLASS()
class  AAbstractHeroCharacter : public APaperCharacter
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperFlipbook* RunningAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperFlipbook* IdleAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

public:
	AAbstractHeroCharacter();

	virtual void Tick(float DeltaSeconds) override;

public:
	UFUNCTION(BlueprintCallable)
	void MoveRight(float Value);

protected:
	void UpdateAnimation();

	void UpdateCharacter();

	void UpdateCamera();
};