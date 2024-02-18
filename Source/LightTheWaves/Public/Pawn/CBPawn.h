// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interface/CBGunInterface.h"
#include "CBPawn.generated.h"

class UWidgetComponent;
class UBoxComponent;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class UPhysicsConstraintComponent;
class UXRDeviceVisualizationComponent;
class UMotionControllerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoUpdated, int32, NewAmmoCount);

UCLASS()
class LIGHTTHEWAVES_API ACBPawn : public APawn, public ICBGunInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACBPawn();
	
	/** Gun Interface */
	virtual void IncreaseCapacity_Implementation(int32 IncreaseAmount) override;
	virtual bool CanReload_Implementation() override;
	virtual void Reload_Implementation() override;
	/** End Gun Interface */

protected:
	UFUNCTION()
	void HookHandBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult);

	UFUNCTION()
	void HookHandEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int OtherBodyIndex);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** The origin of the player in VR */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USceneComponent> VROrigin;

	/** Left Hand */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UMotionControllerComponent> MotionControllerLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UXRDeviceVisualizationComponent> XRDeviceVisualizationLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> PivotLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UPhysicsConstraintComponent> PhysicsConstraintLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UMotionControllerComponent> MotionControllerLeftAim;
	
	/** @NOTE: This might have to become it's own class for encapsulation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> HandCannon;

	/** This determines the spawn location of the projectile */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USceneComponent> ShootLocation;
	/** End Left Hand */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> HMD;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UWidgetComponent> RecenterWidgetComponent;

	/** Right Hand */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMotionControllerComponent> MotionControllerRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UXRDeviceVisualizationComponent> XRDeviceVisualizationRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> PivotRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UPhysicsConstraintComponent> PhysicsConstraintRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UStaticMeshComponent> HandRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UMotionControllerComponent> MotionControllerRightAim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UBoxComponent> RightHandOverlapBox;
	/** End Right Hand */

	/** The default mapping context, can be used for debug to debug input and stuff */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Recenter input action */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> RecenterInputAction;
	
	/** Projectile class to spawn from the cannon */
	UPROPERTY(EditDefaultsOnly, Category = "Cannon")
	TSubclassOf<AActor> ProjectileClass;
	
	/** Currently called from the input action, might have to rework later in order for it work with the cannon physical interaction */
	UFUNCTION(BlueprintCallable)
	virtual void Shoot() override;

	UFUNCTION()
	void RecenterTimer_Elapsed();

	UFUNCTION(Exec)
	void OnRecenterStarted();

	UFUNCTION()
	void OnRecenterEnded();

	UPROPERTY()
	FTimerHandle RecenterTimerHandle;

	UPROPERTY(BlueprintAssignable, Category = "CBPawn")
	FOnAmmoUpdated OnAmmoUpdated;
	
	/** The maximum ammo that the gun can carry by default */
	UPROPERTY(EditDefaultsOnly, Category = "Cannon")
	int32 AmmoCapacity = 5;

	int32 Ammo = 0;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
