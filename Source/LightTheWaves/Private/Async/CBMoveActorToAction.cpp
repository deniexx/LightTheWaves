// Made By Cubic Burrito


#include "Async/CBMoveActorToAction.h"

UCBMoveActorToAction* UCBMoveActorToAction::Execute(UObject* WorldContextObject, AActor* Actor, const FMoveActorToActionData& Data)
{
	UCBMoveActorToAction* Action = NewObject<UCBMoveActorToAction>();
	Action->MoveActorToData = Data;
	Action->MovingActor = Actor;
	Action->RegisterWithGameInstance(WorldContextObject);

	return Action;
}

void UCBMoveActorToAction::Tick(float DeltaTime)
{
	if (LastFrameNumberWeTicked == GFrameCounter || bInterpFinished)
	{
		return;
	}
	
	LastFrameNumberWeTicked = GFrameCounter;
	
	MoveActorToData.InterpAlpha += DeltaTime * (1 / MoveActorToData.InterpDuration);
	const float ActualAlpha = MoveActorToData.Curve ? MoveActorToData.Curve->GetFloatValue(MoveActorToData.InterpAlpha) : MoveActorToData.InterpAlpha;
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
