// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interface/CBLightInteractor.h"
#include "Interface/CBPathingActor.h"
#include "Interface/CBDestroyableObject.h"
#include "CBBoat.generated.h"

class UCBObjectDamageDefinition;
class USphereComponent;

UENUM(BlueprintType)
enum class EBoatPathingState : uint8
{
	FollowingPath,
	FollowingLight,

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
	virtual FOnNewPathChosen& NewPathChosenEvent() override;
	virtual USplineComponent* GetPath_Implementation() override;
	/** End Pathing Actor Interface */

	/** Nav Agent Interface */
	virtual const FNavAgentProperties& GetNavAgentPropertiesRef() const override;
	virtual FVector GetNavAgentLocation() const override;
	/** End Nav Agent Interface */
	
	/** Light Interactor interface */
	virtual void OnLightFocused_Implementation(UPrimitiveComponent* TargetComponent) override;
	virtual void OnLightFocusEnd_Implementation() override;
	/** End Light Interactor interface */

	/**Destroyable Object Interface */
	virtual void TakeDamage_Implementation(AActor* InstigatorActor, EDestroyingObject DestroyingObject, float IncomingDamage) override;
	virtual float GetMaxHealth_Implementation() const override;
	virtual float GetCurrentHealth_Implementation() const override;
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
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BoatMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereTrigger;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> DebrisSpawnLocation;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UInstancedStaticMeshComponent> BoatPathingVis;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USplineComponent> BoatPathingVisSpline;
	
	UPROPERTY()
	TArray<FVector> ReturnToPathPoints;

	UPROPERTY()
	TObjectPtr<USplineComponent> TargetSpline;

	uint32 PointIndex = 0;

	/** The starting health of the boat */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float MaxHealth = 100.f;;

	/** How fast does the boat move */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float MovementSpeed = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float LightFollowSpeed = 500.f;

	/** How much points are rewarded upon reaching the docks */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boat Properties")
	float PointsReward = 100.f;

	/** How much currency is rewarded upon reaching the docks */
	UPROPERTY(EditDefaultsOnly,  BlueprintReadOnly, Category = "Boat Properties")
	float CurrencyReward = 25.f;

	/** How much life is lost when a boat is destroyed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boat Properties")
	float ReputationLoss = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float ReputationRegain = 5.f;

	/** The maximum distance allowed between the boat and path, without having it correct itself(move back towards the path)*/
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float MaxDistanceToPathAllowed = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float SinkingZOffset = 600.f;

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
	float RegeneratePathDelay = 0.1;

	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float RedrawInstancedMeshDelay = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	float CorrectionDistanceOffset = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	TObjectPtr<UStaticMesh> BoatPathingVisMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	TObjectPtr<USoundBase> BoatSpawnSound;
	
	/** The debris actor to be spawned when the boat has been destroyed */
	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	TSubclassOf<AActor> DebrisLeftAfterDestruction;

	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties", meta = (Bitmask, BitmaskEnum="/Script/LightTheWaves.EDestroyingObject"))
	uint8 DebrisLeavingObjects;

	UPROPERTY(EditDefaultsOnly, Category = "Boat Properties")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToQuery;

	UPROPERTY(BlueprintAssignable, Category = "Boat Properties")
	FOnPathingActorLeftPath OnPathingActorLeftPath;

	UPROPERTY(BlueprintAssignable, Category = "Boat Properties")
	FOnNewPathChosen OnNewPathChosen;

	UFUNCTION()
	void DestroyOnFinishedMoving(AActor* MovingActor);

	UFUNCTION()
	void DestroyAfterDither();
	
	void Die(EDestroyingObject DestroyingObject);

private:

	UPROPERTY()
	FNavAgentProperties NavAgentProperties;

	UPROPERTY(VisibleAnywhere, Category = "Boat Properties")
	EBoatPathingState CurrentPathingState = EBoatPathingState::None;

	UPROPERTY()
	FTimerHandle RegeneratePathTimerHandle;

	UPROPERTY()
	FTimerHandle RedrawInstancedMeshTimerHandle;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> FollowTarget;

	UPROPERTY()
	TArray<FVector> TemporaryPathPoints;
	
	UPROPERTY()
	TObjectPtr<USplineComponent> CurrentPath;

	FVector MovementDirection;
	FVector CachedDestination;

	void FollowLight(float DeltaTime);
	void FollowPath(float DeltaTime);
	void VisualisePath();
	void GenerateNewPath();
	void GeneratePathCustom(bool bDrawDebug);
	void GeneratePathUsingNavSystem(bool bDrawDebug);
	void OnPathQueryFinished(uint32 QueryID, ENavigationQueryResult::Type QueryResult, TSharedPtr<FNavigationPath> NavigationPath);
	FVector FindDesiredLocation(bool bDrawDebug);

	void SetState(EBoatPathingState NewState);

	void OrientRotationToMovement(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void LeaveDebris(const FVector& Location);

	void DrawBoatDebugPathing();
	
	EBoatPathingState EvaluateStatePostFollowLight();

	UFUNCTION()
	void RegeneratePath_Elapsed();

	UFUNCTION()
	void AddInstancedMeshesForPathVis();
	
	FORCEINLINE bool IsFollowingLight() const { return CurrentPathingState == EBoatPathingState::FollowingLight; }
	FORCEINLINE bool IsFollowingPath() const { return CurrentPathingState == EBoatPathingState::FollowingPath; }
	FORCEINLINE bool IsNotFollowingLight() const { return CurrentPathingState != EBoatPathingState::FollowingLight; }

	float Health;

	int32 CurrentQueryIndex = INDEX_NONE;
	
public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
