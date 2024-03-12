// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CBMoveActorToAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorFinishedMoving, AActor*, MovingActor);

USTRUCT(BlueprintType)
struct FMoveActorToActionData
{
	GENERATED_BODY()

	friend class UCBMoveActorToAction;

	UPROPERTY(BlueprintReadWrite, Category = "MoveActorToActionData")
	FVector StartLocation = FVector::ZeroVector;

	/** The end location of the Actor */
	UPROPERTY(BlueprintReadWrite, Category = "MoveActorToActionData")
	FVector EndLocation = FVector::ZeroVector;

	/** Should the actor location be used as a start location */
	UPROPERTY(BlueprintReadWrite, Category = "MoveActorToActionData")
	bool bUseActorLocationAsStart = true;

	/** How long should we interpolate for */
	UPROPERTY(BlueprintReadWrite, Category = "MoveActorToActionData")
	float InterpDuration = 1.f;

	/** The interpolation curve to be used, if this is null, Linear Interpolation will be used */
	UPROPERTY(BlueprintReadWrite, Category = "MoveActorToActionData")
	UCurveFloat* Curve = nullptr;

private:

	float InterpAlpha = 0.f;
};

/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API UCBMoveActorToAction : public UBlueprintAsyncActionBase, public FTickableGameObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "MonsterSpawnerAction", meta = (DisplayName = "MoveActorToAction", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UCBMoveActorToAction* Execute(UObject* WorldContextObject, AActor* Actor, FMoveActorToActionData Data);

	virtual void Tick(float DeltaTime) override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;

	UPROPERTY(BlueprintAssignable)
	FOnActorFinishedMoving OnActorFinishedMoving;

private:

	UPROPERTY()
	FMoveActorToActionData MoveActorToData;

	bool bInterpFinished = false;

	UPROPERTY()
	AActor* MovingActor;
	
	uint32 LastFrameNumberWeTicked = INDEX_NONE;
};
