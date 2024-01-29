// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CBPawn.generated.h"

class UInputAction;
class ACBHandCannon;
class UInputMappingContext;
class UCameraComponent;
class UPhysicsConstraintComponent;
class UXRDeviceVisualizationComponent;
class UMotionControllerComponent;

UCLASS()
class LIGHTTHEWAVES_API ACBPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACBPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<USceneComponent> VROrigin;

	/** Left Hand */
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UMotionControllerComponent> MotionControllerLeft;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UXRDeviceVisualizationComponent> XRDeviceVisualizationLeft;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> PivotLeft;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UPhysicsConstraintComponent> PhysicsConstraintLeft;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UMotionControllerComponent> MotionControllerLeftAim;
	
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> HandCannon;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<USceneComponent> ShootLocation;
	/** End Left Hand */

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> HMD;

	/** Right Hand */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UMotionControllerComponent> MotionControllerRight;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UXRDeviceVisualizationComponent> XRDeviceVisualizationRight;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> PivotRight;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UPhysicsConstraintComponent> PhysicsConstraintRight;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> HandRight;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UMotionControllerComponent> MotionControllerRightAim;
	/** End Right Hand */

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ShootInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Cannon")
	TSubclassOf<AActor> ProjectileClass;

	UFUNCTION()
	void Shoot();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
