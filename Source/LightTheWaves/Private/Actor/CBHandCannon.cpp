// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/CBHandCannon.h"


// Sets default values
ACBHandCannon::ACBHandCannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CannonMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("CannonMesh"));
	ShootLocation = CreateDefaultSubobject<USceneComponent>(FName("ShootLocation"));
}

void ACBHandCannon::Shoot()
{
	GetWorld()->SpawnActor<AActor>(ProjectileClass, ShootLocation->GetComponentLocation(), GetActorForwardVector().ToOrientationRotator());
}
