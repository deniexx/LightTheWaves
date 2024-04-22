// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Interface/CBMonsterInterface.h"
#include "CBMonsterHazard.generated.h"

class USplineComponent;

UCLASS()
class LIGHTTHEWAVES_API ACBMonsterHazard : public AActor, public ICBMonsterInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACBMonsterHazard();
	void SubmergeAndDestroy();

	virtual void Destroyed() override;

	/** Monster Interface */
	virtual FOnMonsterDead& OnMonsterDeadEvent() override;
	virtual AActor* GetTarget_Implementation() override;
	virtual bool IsTargeting_Implementation() const override;
	virtual void SetTarget_Implementation(AActor* InTarget) override;
	bool AttemptRelocation();
	/** End Monster Interface */
	
	virtual void PostInitializeComponents() override;

protected:

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnMonsterFinishedSubmerging(AActor* MovingActor);

	UFUNCTION()
	void DestroyAfterSubmerge(AActor* MovingActor);
	
	UFUNCTION()
	void OnNewPathChosen(USplineComponent* NewPath);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> MonsterMesh;

	/** How long should the monster stay on the field */
	UPROPERTY(EditDefaultsOnly, Category = "Monster Props")
	float LifeSpan = 60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Monster Props")
	TArray<int32> ChancesToFollowBoat;

	UPROPERTY(EditDefaultsOnly, Category = "Monster Props")
	FVector2D MinMaxFollowDistanceFromBoat;

	UPROPERTY(EditDefaultsOnly, Category = "Monster Props")
	float MinDistanceToEnd = 600.f;
	
	UPROPERTY()
	int32 TimesFollowedBoat = 0;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleTrigger;

	UPROPERTY()
	AActor* Target;

	UPROPERTY(BlueprintAssignable)
	FOnMonsterDead OnMonsterDead;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayAttackAnimation();

	UPROPERTY()
	FTimerHandle LifeSpanHandle;
};
