// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CBDitherActor.generated.h"

UENUM(BlueprintType)
enum class EDitherMode : uint8
{
	DitherIn,
	DitherOut
};

USTRUCT(BlueprintType)
struct FDitherActorParams
{
	GENERATED_BODY()

	friend class UCBDitherActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DitherActor")
	AActor* TargetActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DitherActor")
	UStaticMeshComponent* TargetMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DitherActor")
	EDitherMode DitherMode = EDitherMode::DitherOut;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DitherActor")
	float Duration = 1.f;

private:

	float DitherAlpha = 0.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDitherFinished);

/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API UCBDitherActor : public UBlueprintAsyncActionBase, public FTickableGameObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "MonsterSpawnerAction", meta = (DisplayName = "DitherActor", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UCBDitherActor* Execute(UObject* WorldContextObject, const FDitherActorParams& DitherActorParams);

	virtual void Activate() override;
	
	virtual void Tick(float DeltaTime) override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;
	
	FOnDitherFinished OnDitherFinished;

private:

	FDitherActorParams Params;

	bool bDitherFinished = false;
	uint32 LastFrameNumberWeTicked = INDEX_NONE;
};
