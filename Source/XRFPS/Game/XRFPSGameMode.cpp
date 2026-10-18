// Copyright XRFPS. All Rights Reserved.

#include "Game/XRFPSGameMode.h"

#include "Player/XRFPSPawn.h"

AXRFPSGameMode::AXRFPSGameMode()
{
	DefaultPawnClass = AXRFPSPawn::StaticClass();
}
