// Made By Cubic Burrito


#include "Pawn/CBPawn.h"

#include "CBBlueprintFunctionLibrary.h"
#include "CBInitialiser.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "XRDeviceVisualizationComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Actor/CBProjectile.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interface/CBHookOverlapInteractor.h"
#include "Kismet/GameplayStatics.h"
#include "LightTheWaves/LightTheWaves.h"

static TAutoConsoleVariable<int32> CVarInfiniteAmmo(
	TEXT("CB.InfiniteAmmo"),
	0,
	TEXT("Enabled/Disables Infinite Ammo")
	TEXT(" 0: Ammo is NOT infinite/n")
	TEXT(" 1: Ammo is infinite/n"),
	ECVF_Cheat
);

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
	RightHandOverlapBox = CreateDefaultSubobject<UBoxComponent>(FName("RightHandOverlapBox"));
	RecenterWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(FName("RecenterWidgetComponent"));
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
	RecenterWidgetComponent->SetupAttachment(Camera);
	HMD->SetupAttachment(Camera);

	MotionControllerRightAim->SetupAttachment(GetRootComponent());
	MotionControllerRight->SetupAttachment(GetRootComponent());
	XRDeviceVisualizationRight->SetupAttachment(MotionControllerRight);
	HandRight->SetupAttachment(MotionControllerRight);
	PivotRight->SetupAttachment(MotionControllerRight);
	PhysicsConstraintRight->SetupAttachment(PivotRight);
	RightHandOverlapBox->SetupAttachment(HandRight);
#pragma endregion

#pragma region Default Values
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
	RecenterWidgetComponent->SetVisibility(false);

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

	RightHandOverlapBox->SetRelativeLocation(FVector(20.f, 0.f, -3.f));
	RightHandOverlapBox->SetBoxExtent(FVector(4.f, 1.f, 2.f));
	
	/** These settings are required to have propertly physical-interacting hands */
	HandRight->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HandRight->SetCollisionObjectType(ECC_PhysicsBody);
	HandRight->SetCollisionResponseToAllChannels(ECR_Block);
	HandRight->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	HandRight->SetCollisionResponseToChannel(ECC_3DWidget, ECR_Ignore);
	HandRight->SetSimulatePhysics(true);
#pragma endregion
	
}

void ACBPawn::IncreaseCapacity_Implementation(int32 IncreaseAmount)
{
	AmmoCapacity += IncreaseAmount;
	OnAmmoUpdated.Broadcast(Ammo, AmmoCapacity);
	UE_LOG(CBLog, Log, TEXT("Ammo Upgrade Bought"))
}

bool ACBPawn::CanReload_Implementation()
{
	return Ammo < AmmoCapacity || MortarAmmo < MortarAmmoCapacity;
}

// Called when the game starts or when spawned
void ACBPawn::BeginPlay()
{
	Super::BeginPlay();

	Ammo = AmmoCapacity;
	/** @NOTE: I'm not sure if it's needed to call these here, but there is no real issue in doing that, so might as well leave them */
	PhysicsConstraintRight->SetConstraintReferenceFrame(EConstraintFrame::Frame1, PivotRight->GetComponentTransform());
	PhysicsConstraintRight->SetConstraintReferenceFrame(EConstraintFrame::Frame2, HandRight->GetComponentTransform());
	PhysicsConstraintLeft->SetConstrainedComponents(PivotLeft, FName(""), HandCannon, FName(""));
	PhysicsConstraintRight->SetConstrainedComponents(PivotRight, FName(""), HandRight, FName(""));
	HandRight->SetSimulatePhysics(true);
	HandCannon->SetSimulatePhysics(true);

	RightHandOverlapBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HookHandBeginOverlap);
	RightHandOverlapBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::HookHandEndOverlap);
	
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Stage);
		UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(0, EOrientPositionSelector::OrientationAndPosition);
		UKismetSystemLibrary::ExecuteConsoleCommand(this, "vr.PixelDensity 1.0");

		if (const APlayerController* PlayerController = GetController<APlayerController>())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	UCBInitialiser* Initialiser = UCBBlueprintFunctionLibrary::GetInitialisationSubsystem(this);
	if (!Initialiser)
	{
		UE_LOG(CBLog, Error, TEXT("Initialiser not available at BeginPlay of CBPawn"));
	}
	else
	{
		Initialiser->RegisterPlayerPawn(this);
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

	/** @NOTE: This is probably only for debug or can be added to have controls for recentering, etc... */
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(RecenterInputAction, ETriggerEvent::Started, this, &ThisClass::OnRecenterStarted);
		EnhancedInputComponent->BindAction(RecenterInputAction, ETriggerEvent::Completed, this, &ThisClass::OnRecenterEnded);
	}
}

void ACBPawn::Reload_Implementation(bool bMortar)
{
	if(bMortar)
	{
		if(StoredMortarAmmo > 0 && MortarAmmo < MortarAmmoCapacity)
		{
			MortarAmmoNeeded = MortarAmmoCapacity - MortarAmmo;
			if(MortarAmmoNeeded <= StoredMortarAmmo)
			{
				MortarAmmo = MortarAmmoCapacity;
				StoredMortarAmmo = StoredMortarAmmo - MortarAmmoNeeded;
			}
			else
			{
				MortarAmmo += StoredMortarAmmo;
				StoredMortarAmmo = 0;
			}
		}
		OnAmmoUpdated.Broadcast(MortarAmmo, MortarAmmoCapacity);
	}
	else
	{
		Ammo = AmmoCapacity;
		OnAmmoUpdated.Broadcast(Ammo, AmmoCapacity);
	}

	bMortarMode = bMortar;
}


void ACBPawn::AddStoredAmmo_Implementation(int32 IncreaseAmount)
{
	StoredMortarAmmo += IncreaseAmount;
}

FOnAmmoExpended& ACBPawn::OnAmmoExpendedEvent()
{
	return OnAmmoExpended;
}

void ACBPawn::HookHandBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp,
                                   int OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if (Other->Implements<UCBHookOverlapInteractor>())
	{
	    ICBHookOverlapInteractor::Execute_OnHookOverlapBegin(Other, RightHandOverlapBox);
	}
}

void ACBPawn::HookHandEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if (Other->Implements<UCBHookOverlapInteractor>())
    {
		ICBHookOverlapInteractor::Execute_OnHookOverlapEnd(Other, RightHandOverlapBox);
    }	
}

void ACBPawn::Shoot()
{
	/** This can be improved, but it's okay for debugging(adding things like aim assist) */
	const bool bInfiniteAmmo = CVarInfiniteAmmo.GetValueOnAnyThread() > 0;
	if (bMortarMode)
	{
		if (MortarAmmo > 0 || bInfiniteAmmo)
		{
			const FTransform SpawnTransform(HandCannon->GetForwardVector().ToOrientationRotator(), ShootLocation->GetComponentLocation());
			ACBProjectile* Actor = GetWorld()->SpawnActorDeferred<ACBProjectile>(ProjectileClassMortar, SpawnTransform, nullptr, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
			const float Power = 1 + FVector::DotProduct(FVector::UpVector, HandCannon->GetForwardVector());
			UProjectileMovementComponent* ProjectileMovementComponent = Actor->GetComponentByClass<UProjectileMovementComponent>();
			ProjectileMovementComponent->Velocity *= Power;
			Actor->FinishSpawning(SpawnTransform);
			if (!bInfiniteAmmo)
			{
				--MortarAmmo;
			}
			OnAmmoUpdated.Broadcast(MortarAmmo, MortarAmmoCapacity);
			UGameplayStatics::PlaySoundAtLocation(this, MortarShootSound, ShootLocation->GetComponentLocation());
		}
		else
		{
			OnAmmoExpended.Broadcast();
		}
		return;
	}
	
	if (Ammo > 0 || bInfiniteAmmo)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(ProjectileClassNormal, ShootLocation->GetComponentLocation(), HandCannon->GetForwardVector().ToOrientationRotator(), SpawnParameters);
		if (!bInfiniteAmmo)
		{
			--Ammo;
		}
		UGameplayStatics::PlaySoundAtLocation(this, NormalShootSound, ShootLocation->GetComponentLocation());
		OnAmmoUpdated.Broadcast(Ammo, AmmoCapacity);
	}
	else
	{
		OnAmmoExpended.Broadcast();
	}
}

FVector ACBPawn::GetLaunchVelocityForProjectile()
{
	FVector Velocity;
	if (bMortarMode)
	{
		const float Power = 1 + FVector::DotProduct(FVector::UpVector, HandCannon->GetForwardVector());
		Velocity = Power * ProjectileClassMortar->GetDefaultObject<ACBProjectile>()->GetLaunchVelocity();
	}
	else
	{
		Velocity = ProjectileClassNormal->GetDefaultObject<ACBProjectile>()->GetLaunchVelocity();
	}

	return (HandCannon->GetForwardVector()) * Velocity.Length();
}

float ACBPawn::GetProjectileLifeSpan()
{
	if (bMortarMode)
	{
		return ProjectileClassMortar->GetDefaultObject<ACBProjectile>()->GetLifetime();
	}
	
	return ProjectileClassNormal->GetDefaultObject<ACBProjectile>()->GetLifetime();
}

float ACBPawn::GetProjectileAreaOfEffect() const
{
	if (bMortarMode)
	{
		return ProjectileClassMortar->GetDefaultObject<ACBProjectile>()->GetAreaOfEffect();
	}
	
	return ProjectileClassNormal->GetDefaultObject<ACBProjectile>()->GetAreaOfEffect();
}

void ACBPawn::OnRecenterStarted()
{
	RecenterWidgetComponent->SetVisibility(true);
	GetWorldTimerManager().SetTimer(RecenterTimerHandle, this, &ThisClass::RecenterTimer_Elapsed, 3.f);
}

void ACBPawn::RecenterTimer_Elapsed()
{
	RecenterWidgetComponent->SetVisibility(false);
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(0, EOrientPositionSelector::OrientationAndPosition);
}

void ACBPawn::OnRecenterEnded()
{
	if (RecenterTimerHandle.IsValid())
	{
		RecenterWidgetComponent->SetVisibility(false);
		GetWorldTimerManager().ClearTimer(RecenterTimerHandle);
	}
}
