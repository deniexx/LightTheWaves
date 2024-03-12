// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CBDisplayTutorialAction.generated.h"

USTRUCT(BlueprintType)
struct FTutorialDisplayParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TutorialDisplayParams")
	AActor* AttachedActor;

	/** Must have text renderer component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TutorialDisplayParams")
	TSubclassOf<AActor> TutorialActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TutorialDisplayParams")
	FString TextToDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TutorialDisplayParams")
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TutorialDisplayParams")
	FVector Offset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TutorialDisplayParams", meta = (sRGB=true))
	FColor TextColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TutorialDisplayParams")
	int32 FontSize = 26;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TutorialDisplayParams")
	USoundBase* SoundToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TutorialDisplayParams")
	float Duration = 5.f;
};

/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API UCBDisplayTutorialAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "MonsterSpawnerAction", meta = (DisplayName = "DisplayTutorial", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UCBDisplayTutorialAction* Execute(UObject* WorldContextObject, const FTutorialDisplayParams& TutorialDisplayParams);

	virtual void Activate() override;

private:

	FTutorialDisplayParams DisplayParams;

	UPROPERTY()
	AActor* SpawnedActor;

	UPROPERTY()
	UObject* WorldContextObject;
};
