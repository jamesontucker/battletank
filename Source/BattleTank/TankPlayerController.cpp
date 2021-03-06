// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"
#include "TankAimingComponent.h"
#include "Tank.h" // so we can implement OnDeath

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!GetPawn()) { return; }
	auto AimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();
	if (!ensure(AimingComponent)) { return; }
	FoundAimingComponent(AimingComponent);
}

void ATankPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedTank = Cast<ATank>(InPawn);
		if (!ensure(PossessedTank)) { return; }

		// Subscribe our local method to the tank's death event
		PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankPlayerController::OnPossessedTankDeath);
	}
}

void ATankPlayerController::OnPossessedTankDeath()
{
	StartSpectatingOnly();
}

void ATankPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimTowardsCrosshair();
}

void ATankPlayerController::AimTowardsCrosshair() 
{
	if (!GetPawn()) { return; } // if not possessing
	auto AimingComponent = GetPawn()->FindComponentByClass<UTankAimingComponent>();
	if (!ensure(AimingComponent)) { return; }

	FVector HitLocation; // OUT param
	if (GetSightRayHitLocation(HitLocation)) { // has a "side-effect, is going to line trace
		AimingComponent->AimAt(HitLocation);
	}
	
}

// Get world location if linetrace through crosshair, true if hits landscape
bool ATankPlayerController::GetSightRayHitLocation(FVector& OutHitLocation) const
{
	// Find the crosshair position
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	
	auto ScreenLocation = FVector2D(ViewportSizeX * CrossHairXLocation, ViewportSizeY * CrossHairYLocation);

	FVector LookDirection;
	FVector LookLocation;
	if (GetLookDirection(ScreenLocation, LookDirection, LookLocation))
	{
		return GetLookVectorHitLocation(LookDirection, LookLocation, OutHitLocation);

	}

	return false;
}

bool ATankPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector &LookDirection, FVector &LookLocation) const
{
	FVector CameraWorldLocation; // To Be Trashed

	return DeprojectScreenPositionToWorld
	(
		ScreenLocation.X,
		ScreenLocation.Y,
		LookLocation,
		LookDirection
	);
}


bool ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector &LookLocation, FVector &HitLocation) const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	FHitResult HitResult;
	FVector EndLocation = LookLocation + (LookDirection * LineTraceRange);
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	FCollisionResponseParams ResponseParameters = FCollisionResponseParams();

	/*DrawDebugLine( // Draw line for debugging line trace
		GetWorld(),
		LookLocation,
		EndLocation,
		FColor(255, 0, 0),
		false, -1, 0,
		5
	);*/


	if (GetWorld()->LineTraceSingleByChannel
	(
		HitResult,
		LookLocation,
		EndLocation,
		ECC_Camera,
		TraceParameters,
		ResponseParameters
	))
	{
		HitLocation = HitResult.Location;
		return true;
	}
	
	return false;
}