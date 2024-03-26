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
	virtual int32 GetCurrency_Implementation() const override;
	virtual int32 GetPoints_Implementation() const override;
	virtual bool HasEnoughCurrency_Implementation(int32 AmountToCheck) override;
	virtual float GetPlayerReputation_Implementation() const override;
	virtual float GetTotalCurrency_Implementation() const override;
	virtual void ApplyChangeToPlayerReputation_Implementation(float Delta) override;
	virtual void IncreaseMaxReputation_Implementation(float IncreaseAmount) override;
	virtual void IncreaseFollowSpeed_Implementation(float IncreaseAmount) override;
	virtual FOnAttributeChanged& OnCurrencyChangedEvent() override;
	virtual FOnAttributeChanged& OnPointsChangedEvent() override;
	virtual FOnReputationChanged& OnReputationChangedEvent() override;
	virtual FOnGameLost& OnGameLostEvent() override;
	/** End Player Interface */
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Reputation")
	float StartingReputation = 100.f;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnCurrencyChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnPointsChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnReputationChanged OnReputationChanged;

	UPROPERTY(BlueprintAssignable)
	FOnGameLost OnGameLost;	

	UPROPERTY()
	int32 TotalCurrency = 0;

	UPROPERTY()
	int32 Currency = 0;

	UPROPERTY()
	int32 Points = 0;

	UPROPERTY()
	float Reputation = 0.f;

	UPROPERTY()
	float MaxReputation = 100.f;
};
