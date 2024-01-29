// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/CBPawn.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "XRDeviceVisualizationComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Actor/CBHandCannon.h"
#include "LightTheWaves/LightTheWaves.h"

// Sets default values
ACBPawn::ACBPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region Component Subobject Creation
	VROrigin = CreateDefaultSubobject<USceneComponent>(FName("VROrigin"));
	MotionControllerLeft = CreateDefaultSubobject<UMotionControllerComponent>(FName("MotionControllerLeft"));
	XRDeviceVisualizationLeft = CreateDefaultSubobject<UXRDeviceVisualizationComponent>(FName("XRDeviceVisualizationLeft"));
	PivotLeft = CreateDefaultSubobject<UStaticMeshComponent>(FName("PivotLeft"));
	PhysicsConstraintLeft = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("PhysicsConstraintLeft"));
	HandCannon = CreateDefaultSubobject<UStaticMeshComponent>(FName("HandCannon"));
	ShootLocation = CreateDefaultSubobject<USceneComponent>(FName("ShootLocation"));
	MotionControllerLeftAim = CreateDefaultSubobject<UMotionControllerComponent>(FName("MotionControllerLeftAim"));
	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	HMD = CreateDefaultSubobject<UStaticMeshComponent>(FName("HMD"));
	MotionControllerRight = CreateDefaultSubobject<UMotionControllerComponent>(FName("MotionControllerRight"));
	XRDeviceVisualizationRight = CreateDefaultSubobject<UXRDeviceVisualizationComponent>(FName("XRDeviceVisualizationRight"));
	PivotRight = CreateDefaultSubobject<UStaticMeshComponent>(FName("PivotRight"));
	PhysicsConstraintRight = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("PhysicsConstraintRight"));
	HandRight = CreateDefaultSubobject<UStaticMeshComponent>(FName("HandRight"));
	MotionControllerRightAim = CreateDefaultSubobject<UMotionControllerComponent>(FName("MotionControllerRightAim"));
#pragma endregion

#pragma region Attachments
	SetRootComponent(VROrigin);
	
	MotionControllerLeftAim->SetupAttachment(GetRootComponent());
	MotionControllerLeft->SetupAttachment(GetRootComponent());
	XRDeviceVisualizationLeft->SetupAttachment(MotionControllerLeft);
	HandCannon->SetupAttachment(MotionControllerLeft);
	ShootLocation->SetupAttachment(HandCannon);
	PivotLeft->SetupAttachment(MotionControllerLeft);
	PhysicsConstraintLeft->SetupAttachment(PivotLeft);

	Camera->SetupAttachment(GetRootComponent());
	HMD->SetupAttachment(Camera);

	MotionControllerRightAim->SetupAttachment(GetRootComponent());
	MotionControllerRight->SetupAttachment(GetRootComponent());
	XRDeviceVisualizationRight->SetupAttachment(MotionControllerRight);
	HandRight->SetupAttachment(MotionControllerRight);
	PivotRight->SetupAttachment(MotionControllerRight);
	PhysicsConstraintRight->SetupAttachment(PivotRight);
#pragma endregion

#pragma region DefaultValues
	MotionControllerLeft->SetTrackingMotionSource(FName("Left"));
	MotionControllerLeft->SetAssociatedPlayerIndex(0);
	PivotLeft->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PivotLeft->SetCollisionResponseToAllChannels(ECR_Ignore);
	PhysicsConstraintLeft->SetDisableCollision(true);
	PhysicsConstraintLeft->SetLinearXLimit(LCM_Free, 0.f);
	PhysicsConstraintLeft->SetLinearYLimit(LCM_Free, 0.f);
	PhysicsConstraintLeft->SetLinearZLimit(LCM_Free, 0.f);
	PhysicsConstraintLeft->SetAngularSwing1Limit(ACM_Free, 45.f);
	PhysicsConstraintLeft->SetAngularSwing2Limit(ACM_Free, 45.f);
	PhysicsConstraintLeft->SetAngularTwistLimit(ACM_Free, 45.f);
	PhysicsConstraintLeft->SetLinearPositionDrive(true, true, true);
	PhysicsConstraintLeft->SetLinearPositionTarget(FVector::ZeroVector);
	PhysicsConstraintLeft->SetLinearVelocityDrive(true, true, true);
	PhysicsConstraintLeft->SetLinearVelocityTarget(FVector::ZeroVector);
	PhysicsConstraintLeft->SetLinearDriveParams(2000.f, 500.f, 0.f);
	PhysicsConstraintLeft->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);
	PhysicsConstraintLeft->SetOrientationDriveTwistAndSwing(true, true);
	PhysicsConstraintLeft->SetOrientationDriveSLERP(false);
	PhysicsConstraintLeft->SetAngularOrientationTarget(FRotator::ZeroRotator);
	PhysicsConstraintLeft->SetAngularVelocityDriveTwistAndSwing(true, true);
	PhysicsConstraintLeft->SetAngularVelocityDriveSLERP(false);
	PhysicsConstraintLeft->SetAngularVelocityTarget(FVector::ZeroVector);
	PhysicsConstraintLeft->SetAngularDriveParams(2000.f, 500.f, 0.f);
	MotionControllerLeftAim->SetTrackingMotionSource(FName("LeftAim"));
	MotionControllerLeft->SetAssociatedPlayerIndex(0);
	PivotLeft->SetHiddenInGame(true);

	/** These settings are required to have propertly physical-interacting hands */
	HandCannon->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HandCannon->SetCollisionObjectType(ECC_PhysicsBody);
	HandCannon->SetCollisionResponseToAllChannels(ECR_Block);
	HandCannon->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	HandCannon->SetCollisionResponseToChannel(ECC_3DWidget, ECR_Ignore);
	HandCannon->SetSimulatePhysics(true);

	Camera->bLockToHmd = true;

	MotionControllerRight->SetTrackingMotionSource(FName("Right"));
	MotionControllerRight->SetAssociatedPlayerIndex(0);
	PivotRight->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PivotRight->SetCollisionResponseToAllChannels(ECR_Ignore);
	PhysicsConstraintRight->SetConstrainedComponents(PivotRight, FName(""), HandRight, FName(""));
	PhysicsConstraintRight->SetDisableCollision(true);
	PhysicsConstraintRight->SetLinearXLimit(LCM_Free, 0.f);
	PhysicsConstraintRight->SetLinearYLimit(LCM_Free, 0.f);
	PhysicsConstraintRight->SetLinearZLimit(LCM_Free, 0.f);
	PhysicsConstraintRight->SetAngularSwing1Limit(ACM_Free, 45.f);
	PhysicsConstraintRight->SetAngularSwing2Limit(ACM_Free, 45.f);
	PhysicsConstraintRight->SetAngularTwistLimit(ACM_Free, 45.f);
	PhysicsConstraintRight->SetLinearPositionDrive(true, true, true);
	PhysicsConstraintRight->SetLinearPositionTarget(FVector::ZeroVector);
	PhysicsConstraintRight->SetLinearVelocityDrive(true, true, true);
	PhysicsConstraintRight->SetLinearVelocityTarget(FVector::ZeroVector);
	PhysicsConstraintRight->SetLinearDriveParams(2000.f, 500.f, 0.f);
	PhysicsConstraintRight->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);
	PhysicsConstraintRight->SetOrientationDriveTwistAndSwing(true, true);
	PhysicsConstraintRight->SetOrientationDriveSLERP(false);
	PhysicsConstraintRight->SetAngularOrientationTarget(FRotator::ZeroRotator);
	PhysicsConstraintRight->SetAngularVelocityDriveTwistAndSwing(true, true);
	PhysicsConstraintRight->SetAngularVelocityDriveSLERP(false);
	PhysicsConstraintRight->SetAngularVelocityTarget(FVector::ZeroVector);
	PhysicsConstraintRight->SetAngularDriveParams(2000.f, 500.f, 0.f);
	MotionControllerRightAim->SetTrackingMotionSource(FName("RightAim"));
	MotionControllerRight->SetAssociatedPlayerIndex(0);
	PivotRight->SetHiddenInGame(true);
	
	/** These settings are required to have propertly physical-interacting hands */
	HandRight->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HandRight->SetCollisionObjectType(ECC_PhysicsBody);
	HandRight->SetCollisionResponseToAllChannels(ECR_Block);
	HandRight->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	HandRight->SetCollisionResponseToChannel(ECC_3DWidget, ECR_Ignore);
	HandRight->SetSimulatePhysics(true);
#pragma endregion
	
}

// Called when the game starts or when spawned
void ACBPawn::BeginPlay()
{
	Super::BeginPlay();
	
	PhysicsConstraintRight->SetConstraintReferenceFrame(EConstraintFrame::Frame1, PivotRight->GetComponentTransform());
	PhysicsConstraintRight->SetConstraintReferenceFrame(EConstraintFrame::Frame2, HandRight->GetComponentTransform());
	PhysicsConstraintLeft->SetConstrainedComponents(PivotLeft, FName(""), HandCannon, FName(""));
	PhysicsConstraintRight->SetConstrainedComponents(PivotRight, FName(""), HandRight, FName(""));
	HandRight->SetSimulatePhysics(true);
	HandCannon->SetSimulatePhysics(true);


	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Stage);
		UKismetSystemLibrary::ExecuteConsoleCommand(this, "vr.PixelDensity 1.0");

		if (const APlayerController* PlayerController = GetController<APlayerController>())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

// Called every frame
void ACBPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACBPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/** @NOTE: We might want to setup some keys for debugging, like firing the cannon? */
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ShootInputAction, ETriggerEvent::Started, this, &ThisClass::Shoot);
	}
}

void ACBPawn::Shoot()
{
	GetWorld()->SpawnActor<AActor>(ProjectileClass, ShootLocation->GetComponentLocation(), HandCannon->GetForwardVector().ToOrientationRotator());
}
