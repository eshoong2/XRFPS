// Copyright XRFPS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XRInputSource.generated.h"

struct FXRInputFrame;
class UEnhancedInputComponent;
class UInputMappingContext;

/**
 * 입력 소스의 부모 클래스. "FXRInputFrame을 만들어라"라는 약속만 정한다.
 *
 * 실제 입력은 파생 클래스가 만든다.
 * - UXRDesktopInputSource: 키보드·마우스로 머리와 양손을 흉내
 * - (M1 6단계) UXROpenXRInputSource: Quest
 * - (나중) UXRReplayInputSource: 녹화한 프레임 재생
 *
 * Pawn에 여러 입력 소스를 붙여 두면 UXRPoseComponent가 지금 쓸 수 있는 것(IsAvailable)을 고른다.
 * 설계: docs/decisions/ADR-0004-m1-pawn-input-architecture.md 결정 2
 */
UCLASS(Abstract)
class XRFPS_API UXRInputSource : public UActorComponent
{
	GENERATED_BODY()

public:
	UXRInputSource();

	/** 지금 이 입력 소스를 쓸 수 있는가. 예: OpenXR 소스는 헤드셋이 켜져 있을 때만 true */
	virtual bool IsAvailable() const { return false; }

	/** 디버그 화면에 표시할 이름 */
	virtual FString GetSourceName() const { return TEXT("None"); }

	/**
	 * 프레임의 포즈를 Camera·MotionController 컴포넌트에 직접 적용해야 하는가.
	 * - 데스크톱, 녹화 재생: 엔진이 컴포넌트를 움직여 주지 않으므로 true
	 * - OpenXR: 엔진이 헤드셋·컨트롤러를 따라 컴포넌트를 움직이므로 false (M1 6단계에서 override)
	 */
	virtual bool ShouldApplyFrameToComponents() const { return true; }

	/** 이 소스가 쓰는 입력 매핑. Pawn이 플레이어의 입력 서브시스템에 등록한다. 없으면 nullptr */
	virtual UInputMappingContext* GetMappingContext() { return nullptr; }

	/** 입력 액션을 함수에 연결한다. Pawn이 입력을 준비할 때 한 번 호출한다. */
	virtual void BindInput(UEnhancedInputComponent* EnhancedInput) {}

	/**
	 * 이번 프레임 입력을 OutFrame에 채운다. 파생 클래스가 반드시 구현한다.
	 * FrameNumber와 TimeSeconds는 호출하는 쪽(UXRPoseComponent)이 채운다.
	 */
	virtual void BuildFrame(FXRInputFrame& OutFrame) PURE_VIRTUAL(UXRInputSource::BuildFrame, );
};
