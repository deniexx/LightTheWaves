// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interface/CBPlayerInterface.h"
#include "CBPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API ACBPlayerState : public APlayerState, public ICBPlayerInterface
{
	GENERATED_BODY()

public:

	/** Player Interface */
	virtual void ApplyChangeToCurrency_Implementation(int32 Delta) override;
	virtual void ApplyChangeToPoints_Implementation(int32 Delta) override;
	virtual FOnAttributeChanged& OnCurrencyChangedEvent() override;
	virtual FOnAttributeChanged& OnPointsChangedEvent() override;
	/** End Player Interface */
	
protected:

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnCurrencyChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnPointsChanged;

	UPROPERTY()
	int32 Currency = 0;

	UPROPERTY()
	int32 Points = 0;
};
