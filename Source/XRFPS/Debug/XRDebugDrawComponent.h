// Copyright XRFPS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XRDebugDrawComponent.generated.h"

/**
 * 개발용 디버그 표시를 한곳에 모은 컴포넌트.
 *
 * - 머리, 양손의 Grip·Aim 축
 * - 입력 상태줄 (입력 소스, 프레임 번호, 주 손 트리거, 트래킹)
 * - (M1 3단계 이후) 총열 선, 양손 파지 허용 범위
 *
 * 콘솔 명령 xr.Debug.Draw 0 / 1 로 끄고 켠다.
 * 게임 로직은 넣지 않는다. 여기서 보이는 건 "게임이 실제로 쓰는 값"이어야 하므로
 * 화면에 보이는 컴포넌트(MotionController, Camera)의 Transform을 그대로 그린다.
 */
UCLASS(ClassGroup = (XR), meta = (BlueprintSpawnableComponent))
class XRFPS_API UXRDebugDrawComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UXRDebugDrawComponent();

	/** Pawn Tick 마지막에 호출한다. 이번 프레임 갱신이 모두 끝난 상태를 그린다. */
	void DrawDebug() const;
};
