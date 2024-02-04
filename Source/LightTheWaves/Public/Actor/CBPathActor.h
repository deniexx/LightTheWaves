// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/CBPath.h"
#include "CBPathActor.generated.h"

UCLASS()
class LIGHTTHEWAVES_API ACBPathActor : public AActor, public ICBPath
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACBPathActor();

	/** CBPath interface */
	virtual USplineComponent* GetPath_Implementation() override;
	virtual int32 GetNumberOfBoatsOnPath_Implementation() override;
	virtual void RegisterBoatOnPath_Implementation(AActor* Boat) override;
	/** End CBPath interface */

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPathingActorLeftPath(AActor* PathingActor);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USplineComponent* SplineComponent;

	UPROPERTY()
	int32 PathingActorsOnPath = 0;
};
