// Copyright XRFPS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Input/XRInputFrame.h"
#include "XRPoseComponent.generated.h"

class UXRInputSource;

/**
 * 이번 프레임의 입력(FXRInputFrame)을 한 곳에 모아 두는 컴포넌트.
 *
 * - 같은 액터에 붙은 입력 소스 중 지금 쓸 수 있는 것을 골라 프레임을 만든다.
 * - 손, 총 같은 다른 컴포넌트는 입력 소스가 아니라 이 컴포넌트에서 입력을 읽는다.
 * - M2에서 서버로 보낼 데이터가 바로 이 프레임이다.
 *
 * 스스로 Tick하지 않는다. 소유 Pawn이 Tick 맨 앞에서 UpdateFrame()을 호출해
 * "입력 갱신 → 카메라·손·총 갱신" 순서를 코드로 분명하게 보이게 한다.
 */
UCLASS(ClassGroup = (XR), meta = (BlueprintSpawnableComponent))
class XRFPS_API UXRPoseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UXRPoseComponent();

	/** 지금 쓸 입력 소스를 고르고 이번 프레임 입력을 만든다. */
	void UpdateFrame();

	/** 가장 최근에 만든 입력 프레임 */
	const FXRInputFrame& GetCurrentFrame() const { return CurrentFrame; }

	/** 지금 쓰고 있는 입력 소스. 고르지 못했으면 nullptr */
	UXRInputSource* GetActiveSource() const { return ActiveSource; }

	/** 같은 액터에 붙은 입력 소스 중 지금 쓸 수 있는 첫 번째를 고른다. */
	UXRInputSource* SelectAvailableSource() const;

private:
	/** 머리·양손 축과 입력 상태를 화면에 그린다. (M1 2단계에서 디버그 컴포넌트로 옮길 예정) */
	void DrawDebugPoses() const;

	UPROPERTY(Transient)
	TObjectPtr<UXRInputSource> ActiveSource;

	FXRInputFrame CurrentFrame;

	int32 NextFrameNumber = 0;
};
