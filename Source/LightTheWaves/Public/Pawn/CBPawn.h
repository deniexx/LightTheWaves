// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interface/CBGunInterface.h"
#include "CBPawn.generated.h"

class ACBProjectile;
class UWidgetComponent;
class UBoxComponent;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class UPhysicsConstraintComponent;
class UXRDeviceVisualizationComponent;
class UMotionControllerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoUpdated, int32, NewAmmoCount, int32, MaxAmmo);

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
	virtual void Reload_Implementation(bool bMortar) override;
	virtual void MortarReload_Implementation() override;
	virtual void AddStoredAmmo_Implementation(int32 IncreaseAmount) override;
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

	UPROPERTY(EditDefaultsOnly, Category = "Cannon")
	TObjectPtr<USoundBase> NormalShootSound;

	UPROPERTY(EditDefaultsOnly, Category = "Cannon")
	TObjectPtr<USoundBase> MortarShootSound;
	
	/** Projectile class to spawn from the cannon */
	UPROPERTY(EditDefaultsOnly, Category = "Cannon")
	TSubclassOf<ACBProjectile> ProjectileClassNormal;

	/** Projectile class to spawn from the cannon */
	UPROPERTY(EditDefaultsOnly, Category = "Cannon")
	TSubclassOf<ACBProjectile> ProjectileClassMortar;
	
	/** Currently called from the input action, might have to rework later in order for it work with the cannon physical interaction */
	UFUNCTION(BlueprintCallable)
	virtual void Shoot() override;

	//UFUNCTION(BlueprintCallable)
	//virtual void SwitchAmmoType() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual FVector GetLaunchVelocityForProjectile() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual float GetProjectileLifeSpan() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetProjectileAreaOfEffect() const;
	
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

	UPROPERTY(EditDefaultsOnly, Category = "Cannon")
	int32 MortarAmmoCapacity = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Cannon")
	int32 MortarAmmo = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Cannon")
	int32 Ammo = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Cannon")
	int32 StoredMortarAmmo = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Cannon")
	int32 MortarAmmoNeeded;
	
	UPROPERTY(BlueprintReadOnly)
	bool bMortarMode = false;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
