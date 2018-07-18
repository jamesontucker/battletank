// Fill out your copyright notice in the Description page of Project Settings.

#include "TankBarrel.h"


void UTankBarrel::Elevate(float DegreesPerSecond)
{
	// Move the barrel the right amount this frame
	UE_LOG(LogTemp, Warning, TEXT("Barrel elevate speed: %f"), DegreesPerSecond);

	// Given a Max elevation speed, and the frame time
}