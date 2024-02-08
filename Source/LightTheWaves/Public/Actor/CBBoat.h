// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interface/CBLightInteractor.h"
#include "Interface/CBPathingActor.h"
#include "Interface/CBDestroyableObject.h"
#include "CBBoat.generated.h"

class USphereComponent;

UENUM(BlueprintType)
enum class EBoatPathingState
{
	FollowingPath,
	FollowingLight,
	ReturningToPath,

	None UMETA( Hidden )
};

UCLASS()
class LIGHTTHEWAVES_API ACBBoat : public APawn, public ICBLightInteractor, public ICBPathingActor, public ICBDestroyableObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACBBoat();
	
	/** Pathing Actor Interface */
	virtual void SetPath_Implementation(USplineComponent* NewPath) override;
	virtual FOnPathingActorLeftPath& PathingActorLeftPathEvent() override;
	/** End Pathing Actor Interface */
	
	/** Light Interactor interface */
	virtual void OnLightFocused_Implementation(UPrimitiveComponent* TargetComponent) override;
	virtual void OnLightFocusEnd_Implementation() override;
	/** End Light Interactor interface */

	/**Destroyable Object Interface */
	virtual void OnDestroyed_Implementation(AActor* DestroyedActor) override;
	/**End Destroyable Object interface */

#if WITH_EDITORONLY_DATA	
	/** When TRUE, updating the RatioCurrencyToPoints variable will update the currency based on the points
	  * When FALSE it will update the points based on the currency */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	bool bUpdateBasedOnPoints = false;

	/** How much points is one currency(e.g 4 means that 1 currency is worth 4 points(Currency * Ratio) and 1 point is worth 0.25 currency(Points * (1 / Ratio)))
	 * Changing this will update the values using the ratio based on the bUpdateRatioBasedOnPoints boolean value */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float RatioCurrencyToPoints = 4;
#endif

#if WITH_EDITOR
#if !WITH_EDITORONLY_DATA	
	bool bUpdateBasedOnPoints = false;
	float RatioCurrencyToPoints = 4;
#endif

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BoatMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereTrigger;

	UPROPERTY()
	TArray<FVector> ReturnToPathPoints;

	uint32 PointIndex = 0;

	/** How fast does the boat move */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float MovementSpeed = 50.f;

	/** How much points are rewarded upon reaching the docks */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float PointsReward = 100.f;

	/** How much currency is rewarded upon reaching the docks */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float CurrencyReward = 25.f;

	/** How much life is lost when a boat is destroyed */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float LifeLoss = 10.f;

	/** The maximum distance allowed between the boat and path, without having it correct itself(move back towards the path)*/
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float MaxDistanceToPathAllowed = 50.f;

	/** How much further forward should the actor choose a point on the path, when returning to it
	 * 0 means it will make as straight of a line as possible to the nearest point on the spline, not make any horizontal progress along it
	 * > 0 means that the boat will still in a sense continue moving forward, but also towards the spline at the same time
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float ReturnToPathForwardLook = 150.f;

	/** How fast does this actor rotation in the direction of which it is moving */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float RotationRate = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	TSubclassOf<AActor> DebrisLeftAfterDestruction;

	UPROPERTY(BlueprintAssignable, Category = "Boat Properties")
	FOnPathingActorLeftPath OnPathingActorLeftPath;

private:

	EBoatPathingState CurrentPathingState = EBoatPathingState::None;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> FollowTarget;

	UPROPERTY()
	TArray<FVector> TemporaryPathPoints;
	
	UPROPERTY()
	TObjectPtr<USplineComponent> CurrentPath;

	FVector MovementDirection;

	void FollowLight(float DeltaTime);
	void FollowPath(float DeltaTime);
	void ReturnToPath(float DeltaTime);

	void OrientRotationToMovement(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void LeaveDebris(const FVector& Location);

	void DrawBoatDebugPathing();
	
	EBoatPathingState EvaluateStatePostFollowLight();
	
	FORCEINLINE bool IsFollowingLight() const { return CurrentPathingState == EBoatPathingState::FollowingLight; }
	FORCEINLINE bool IsFollowingPath() const { return CurrentPathingState == EBoatPathingState::FollowingPath; }
	FORCEINLINE bool IsReturningToPath() const { return CurrentPathingState == EBoatPathingState::ReturningToPath; }
	FORCEINLINE bool IsNotFollowingLight() const { return CurrentPathingState != EBoatPathingState::FollowingLight; }

	
public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
