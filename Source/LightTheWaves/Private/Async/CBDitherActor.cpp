// Made By Cubic Burrito


#include "Async/CBDitherActor.h"

#include "Kismet/GameplayStatics.h"

UCBDitherActor* UCBDitherActor::Execute(UObject* WorldContextObject, const FDitherActorParams& DitherActorParams)
{
	UCBDitherActor* DitherActor = NewObject<UCBDitherActor>();
	DitherActor->Params = DitherActorParams;
	DitherActor->Params.DitherAlpha = DitherActor->Params.DitherMode == EDitherMode::DitherIn ? 0.f : 1.f;
	
	DitherActor->RegisterWithGameInstance(UGameplayStatics::GetGameInstance(WorldContextObject));
	DitherActor->Activate();

	return DitherActor;
}

void UCBDitherActor::Activate()
{
	Super::Activate();

	const int32 MaterialsNum = Params.TargetMesh->GetNumMaterials();
	for (int32 i = 0; i < MaterialsNum; ++i)
	{
		Params.TargetMesh->CreateDynamicMaterialInstance(i);
	}
}

void UCBDitherActor::Tick(float DeltaTime)
{
	if (LastFrameNumberWeTicked == GFrameCounter || bDitherFinished || !Params.TargetActor)
	{
		if (!Params.TargetActor)
		{
			SetReadyToDestroy();
		}
		
		return;
	}

	LastFrameNumberWeTicked = GFrameCounter;

	if (Params.DitherMode == EDitherMode::DitherIn)
	{
		Params.DitherAlpha += DeltaTime * (1 / Params.Duration);
	}
	else
	{
		Params.DitherAlpha -= DeltaTime * (1 / Params.Duration);
	}
	
	Params.TargetMesh->SetScalarParameterValueOnMaterials("Alpha", Params.DitherAlpha);

	if (Params.DitherAlpha < 0.f || Params.DitherAlpha > 1.f)
	{
		bDitherFinished = true;
		SetReadyToDestroy();
	}
}

UWorld* UCBDitherActor::GetTickableGameObjectWorld() const
{
	return Params.TargetActor ? Params.TargetActor->GetWorld() : nullptr;
}

bool UCBDitherActor::IsTickableWhenPaused() const
{
	return false;
}

TStatId UCBDitherActor::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCBMoveActorToAction, STATGROUP_Tickables);;
}
