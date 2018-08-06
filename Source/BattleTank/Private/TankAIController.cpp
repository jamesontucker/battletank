// Fill out your copyright notice in the Description page of Project Settings.

#include "TankAIController.h"
#include "TankAimingComponent.h"
#include "Tank.h" // so we can implement OnDeath

// Depends on movement component via pathfinding system

void ATankAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ATankAIController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedTank = Cast<ATank>(InPawn);
		if (!ensure(PossessedTank)) { return; }

		// Subscribe our local method to the tank's death event
		PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankAIController::OnPossessedTankDeath);
	}
}

void ATankAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto PlayerTank = GetWorld()->GetFirstPlayerController()->GetPawn();
	auto ControlledTank = GetPawn();

	if (!ensure(PlayerTank && ControlledTank)) { return; }

	//Move towards the player
	MoveToActor(PlayerTank, AcceptanceRadius);

	//Aim towards the player
	auto AimingComponent = ControlledTank->FindComponentByClass<UTankAimingComponent>();
	AimingComponent->AimAt(PlayerTank->GetActorLocation());

	//Fire if locked
	if (AimingComponent->GetFiringState() == EFiringStatus::Locked)
	{
		AimingComponent->Fire();
	}
	
}

void ATankAIController::OnPossessedTankDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("The Tank Died"));
}