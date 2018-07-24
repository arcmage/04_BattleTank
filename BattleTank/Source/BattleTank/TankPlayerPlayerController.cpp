// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerPlayerController.h"
#include "Engine/World.h"

void ATankPlayerPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto ControlledTank = GetContrtolledTank();
	if (ControlledTank) {
		UE_LOG(LogTemp, Warning, TEXT("Tank named %s was possessed"), *ControlledTank->GetName())
	}
	else { UE_LOG(LogTemp, Warning, TEXT("No Tank was possessed")) }
}

void ATankPlayerPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimTowardCrosshair();
}

ATank* ATankPlayerPlayerController::GetContrtolledTank() const
{
	return Cast<ATank>(GetPawn());
}

void ATankPlayerPlayerController::AimTowardCrosshair()
{
	if (!GetContrtolledTank()) { return; }

	FVector HitLocation;	// Out parameter
	if (GetSightRayHitLocation(HitLocation))	// Has "side-effect", is going to line trace
	{
		GetContrtolledTank()->AimAt(HitLocation);
	}
}

// Get world location of linetrace crosshair, true if hits landscape
bool ATankPlayerPlayerController::GetSightRayHitLocation(FVector& HitLocation) const
{
	// Find the crosshair position in pixel cooridnate
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrossHairXLocation, ViewportSizeY * CrossHairYLocation);
	FVector LookDirection;

	if(GetLookDirection(ScreenLocation, LookDirection))
	{
		GetLookVectorHitLocation(LookDirection, HitLocation);
	}


	return true;
}

bool ATankPlayerPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector WorldLocation; // To be discarded
	return DeprojectScreenPositionToWorld(ScreenLocation.X, ScreenLocation.Y, WorldLocation, LookDirection);
}

bool ATankPlayerPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector& HitLocation) const
{
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);
	if (GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartLocation,
			EndLocation,
			ECollisionChannel::ECC_Visibility)
		)
	{
		HitLocation = HitResult.Location;
		return true;
	}
	HitLocation = FVector(0);
	return false;
}
