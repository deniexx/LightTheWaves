// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interface/CBLightInteractor.h"
#include "Interface/CBPathingActor.h"
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
class LIGHTTHEWAVES_API ACBBoat : public APawn, public ICBLightInteractor, public ICBPathingActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACBBoat();
	
	/** Pathing Actor Interface */
	virtual void SetPath_Implementation(USplineComponent* NewPath) override;
	/** End Pathing Actor Interface */
	
	/** Light Interactor interface */
	virtual void OnLightFocused_Implementation(UPrimitiveComponent* TargetComponent) override;
	virtual void OnLightFocusEnd_Implementation() override;
	/** End Light Interactor interface */

#if WITH_EDITORONLY_DATA	
	/** When TRUE, updating the RatioCurrencyToPoints variable will update the currency based on the points
	  * When FALSE it will update the points based on the currency */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Stats")
	bool bUpdateBasedOnPoints = false;

	/** How much points is one currency(e.g 4 means that 1 currency is worth 4 points(Currency * Ratio) and 1 point is worth 0.25 currency(Points * (1 / Ratio)))
	 * Changing this will update the values using the ratio based on the bUpdateRatioBasedOnPoints boolean value */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Stats")
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

	UPROPERTY(VisibleAnywhere, Category = "Boat Stats")
	TObjectPtr<UStaticMeshComponent> BoatMesh;

	UPROPERTY(VisibleAnywhere, Category = "Boat Stats")
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

	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	TSubclassOf<AActor> DebrisLeftAfterDestruction;

private:

	EBoatPathingState CurrentPathingState = EBoatPathingState::None;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> FollowTarget;

	UPROPERTY()
	TArray<FVector> TemporaryPathPoints;
	
	UPROPERTY()
	TObjectPtr<USplineComponent> CurrentPath;

	void FollowLight(float DeltaTime);
	void FollowPath(float DeltaTime);
	void ReturnToPath(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void LeaveDebris(const FVector& Location);

	void DrawBoatDebugPathing(const FVector& Direction);
	
	EBoatPathingState EvaluateStatePostFollowLight();
	
	FORCEINLINE bool IsFollowingLight() const { return CurrentPathingState == EBoatPathingState::FollowingLight; }
	FORCEINLINE bool IsFollowingPath() const { return CurrentPathingState == EBoatPathingState::FollowingPath; }
	FORCEINLINE bool IsReturningToPath() const { return CurrentPathingState == EBoatPathingState::ReturningToPath; }
	FORCEINLINE bool IsNotFollowingLight() const { return CurrentPathingState != EBoatPathingState::FollowingLight; }

	
public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
