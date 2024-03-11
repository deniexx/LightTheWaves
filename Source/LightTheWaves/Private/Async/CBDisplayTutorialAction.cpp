// Made By Cubic Burrito


#include "Async/CBDisplayTutorialAction.h"

#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"

UCBDisplayTutorialAction* UCBDisplayTutorialAction::Execute(UObject* WorldContextObject,
                                                            const FTutorialDisplayParams& TutorialDisplayParams)
{
	UCBDisplayTutorialAction* Action = NewObject<UCBDisplayTutorialAction>();
	Action->DisplayParams = TutorialDisplayParams;
	Action->WorldContextObject = WorldContextObject;
	
	Action->RegisterWithGameInstance(UGameplayStatics::GetGameInstance(WorldContextObject));
	Action->Activate();
	return Action;
}

void UCBDisplayTutorialAction::Activate()
{
	Super::Activate();

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return;
	}

	const FVector Location = DisplayParams.AttachedActor->GetActorLocation() + DisplayParams.Offset;
	const FRotator Rotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	SpawnedActor = World->SpawnActor<AActor>(DisplayParams.TutorialActor, Location, Rotation, SpawnParameters);

	if (UTextRenderComponent* TextRenderComponent = SpawnedActor->GetComponentByClass<UTextRenderComponent>())
	{
		TextRenderComponent->SetText(FText::FromString(DisplayParams.TextToDisplay));
		TextRenderComponent->SetTextRenderColor(DisplayParams.TextColor);
		SpawnedActor->SetLifeSpan(DisplayParams.Duration);
		UGameplayStatics::PlaySoundAtLocation(WorldContextObject, DisplayParams.SoundToPlay, Location);
		return;
	}

	SpawnedActor->Destroy();
	SetReadyToDestroy();
}
