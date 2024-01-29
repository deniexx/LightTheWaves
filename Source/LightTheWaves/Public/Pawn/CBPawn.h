// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CBPawn.generated.h"

class UInputAction;
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

	/** The origin of the player in VR */
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
	
	/** @NOTE: This might have to become it's own class for encapsulation */
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> HandCannon;

	/** This determines the spawn location of the projectile */
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

	/** The default mapping context, can be used for debug to debug input and stuff */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Shoot input action for debug purposes */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ShootInputAction;

	/** Projectile class to spawn from the cannon */
	UPROPERTY(EditDefaultsOnly, Category = "Cannon")
	TSubclassOf<AActor> ProjectileClass;

	/** Currently called from the input action, might have to rework later in order for it work with the cannon physical interaction */
	UFUNCTION()
	void Shoot();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
