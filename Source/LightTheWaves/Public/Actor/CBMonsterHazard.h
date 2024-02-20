// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Interface/CBMonsterInterface.h"
#include "CBMonsterHazard.generated.h"

UCLASS()
class LIGHTTHEWAVES_API ACBMonsterHazard : public AActor, public ICBMonsterInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACBMonsterHazard();

	virtual void Destroyed() override;

	/** Monster Interface */
	virtual FOnMonsterDead& OnMonsterDeadEvent() override;
	virtual AActor* GetTarget_Implementation() override;
	virtual bool IsTargeting_Implementation() const override;
	virtual void SetTarget_Implementation(AActor* InTarget) override;
	/** End Monster Interface */
	
	virtual void PostInitializeComponents() override;

protected:
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MonsterMesh;

	/** How long should the monster stay on the field */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	float LifeSpan = 60.f;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleTrigger;

	UPROPERTY()
	AActor* Target;

	UPROPERTY(BlueprintAssignable)
	FOnMonsterDead OnMonsterDead;
	

};
