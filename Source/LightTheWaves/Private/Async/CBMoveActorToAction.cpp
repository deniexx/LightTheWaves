// Made By Cubic Burrito


#include "Async/CBMoveActorToAction.h"

#include "LightTheWaves/LightTheWaves.h"

UCBMoveActorToAction* UCBMoveActorToAction::Execute(UObject* WorldContextObject, AActor* Actor, FMoveActorToActionData Data)
{
	UCBMoveActorToAction* Action = NewObject<UCBMoveActorToAction>();
	
	if (Data.bUseActorLocationAsStart)
	{
		Data.StartLocation = Actor->GetActorLocation();
	}

	Data.InterpAlpha = 0.f;
	Action->MoveActorToData = Data;
	Action->MovingActor = Actor;
	Action->RegisterWithGameInstance(WorldContextObject);

	return Action;
}

void UCBMoveActorToAction::Tick(float DeltaTime)
{
	if (LastFrameNumberWeTicked == GFrameCounter || bInterpFinished || !MovingActor)
	{
		if (!MovingActor)
		{
			SetReadyToDestroy();
		}
		
		return;
	}
	
	LastFrameNumberWeTicked = GFrameCounter;
	
	MoveActorToData.InterpAlpha += DeltaTime * (1 / MoveActorToData.InterpDuration);
	float ActualAlpha;
	if (MoveActorToData.Curve != nullptr)
	{
		ActualAlpha = MoveActorToData.Curve->GetFloatValue(MoveActorToData.InterpAlpha);
	}
	else
	{
		ActualAlpha = MoveActorToData.InterpAlpha;
	}
	const FVector NewLocation = FMath::Lerp(MoveActorToData.StartLocation, MoveActorToData.EndLocation, ActualAlpha);
	
	MovingActor->SetActorLocation(NewLocation);

	if (MoveActorToData.InterpAlpha >= 1.f)
	{
		OnActorFinishedMoving.Broadcast(MovingActor);
		SetReadyToDestroy();
		bInterpFinished = true;
	}
}

UWorld* UCBMoveActorToAction::GetTickableGameObjectWorld() const
{
	return MovingActor ? MovingActor->GetWorld() : nullptr;
}

bool UCBMoveActorToAction::IsTickableWhenPaused() const
{
	return false;
}

TStatId UCBMoveActorToAction::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCBMoveActorToAction, STATGROUP_Tickables);;
}
