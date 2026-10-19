// Copyright XRFPS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "XRFPSGameMode.generated.h"

/**
 * XRFPS 기본 게임 모드. M1에서는 플레이어 Pawn 지정만 한다.
 * 라운드, 총 선택 등은 M2 이후.
 */
UCLASS()
class XRFPS_API AXRFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AXRFPSGameMode();
};
