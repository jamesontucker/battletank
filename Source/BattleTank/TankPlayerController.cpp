// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"
#include "Tank.h"

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto ControlledTank = GetControlledTank();

	if (!ControlledTank) {
		UE_LOG(LogTemp, Warning, TEXT("PlayerController not possesing a tank"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("PlayerController possessing: %s"), *(ControlledTank->GetName()));
	}

}

void ATankPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimTowardsCrosshair();
}

ATank* ATankPlayerController::GetControlledTank() const 
{
	return Cast<ATank>(GetPawn());
}

void ATankPlayerController::AimTowardsCrosshair() 
{
	if (!GetControlledTank()) { return; }

	FVector HitLocation; // OUT param
	if (GetSightRayHitLocation(HitLocation)) { // has a "side-effect, is going to line trace
		GetControlledTank()->AimAt(HitLocation);
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
		GetLookVectorHitLocation(LookDirection, LookLocation, OutHitLocation);

	}
	//    UE_LOG(LogTemp,Warning,TEXT("LookDirection: %s"), *LookDirection.ToString())
	//OutHitLocation = FVector(1.0f);
	return true;
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

	/*DrawDebugLine(
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
		ECC_Visibility,
		TraceParameters,
		ResponseParameters
	))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Captain We've Hit something"))
		HitLocation = HitResult.Location;
		return true;
	}
	
	return false;
}
