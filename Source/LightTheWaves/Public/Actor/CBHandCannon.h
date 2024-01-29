// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CBHandCannon.generated.h"

UCLASS()
class LIGHTTHEWAVES_API ACBHandCannon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACBHandCannon();

	UFUNCTION(BlueprintCallable)
	void Shoot();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CannonMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> ShootLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor> ProjectileClass;
};
