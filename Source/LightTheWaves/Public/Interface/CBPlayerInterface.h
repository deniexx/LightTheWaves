// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CBPlayerInterface.generated.h"

USTRUCT(BlueprintType)
struct FGameLostData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameLostData")
	FString LoseReason;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChanged, float, OldAmount, float, NewAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnReputationChanged, float, OldAmount, float, NewAmount, float, MaxAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameLost, const FGameLostData&, Data);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCBPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIGHTTHEWAVES_API ICBPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Attributes")
	void ApplyChangeToCurrency(int32 Delta);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Attributes")
	void ApplyChangeToPoints(int32 Delta);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Attributes")
	int32 GetPoints() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Attributes")
	int32 GetCurrency() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Attributes")
	bool HasEnoughCurrency(int32 AmountToCheck);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Reputation")
	float GetPlayerReputation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Attributes")
	void ApplyChangeToPlayerReputation(float Delta);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Attributes")
	void IncreaseMaxReputation(float IncreaseAmount);	

	virtual FOnAttributeChanged& OnPointsChangedEvent() = 0;
	virtual FOnAttributeChanged& OnCurrencyChangedEvent() = 0;
	virtual FOnReputationChanged& OnReputationChangedEvent() = 0;
	virtual FOnGameLost& OnGameLostEvent() = 0;
};
