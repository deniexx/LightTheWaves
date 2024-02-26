// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CBGunInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCBGunInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIGHTTHEWAVES_API ICBGunInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Reload();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanReload();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IncreaseCapacity(int32 IncreaseAmount = 1);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddStoredAmmo(int32 IncreaseAmount = 1);

protected:

	virtual void Shoot() = 0;
	virtual void SwitchAmmoType() = 0;
	virtual FVector GetLaunchVelocityForProjectile() = 0;
	virtual float GetProjectileLifeSpan() = 0;
	
};
