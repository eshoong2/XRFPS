// Copyright XRFPS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "XRFPSPawn.generated.h"

class UCameraComponent;
class UXRDesktopInputSource;
class UXRPoseComponent;

/**
 * XR 플레이어 Pawn.
 *
 * M1 1단계에서는 입력 확인에 필요한 최소 구성만 둔다:
 * VROrigin, Camera, 입력 소스(데스크톱), Pose 컴포넌트.
 * 손 컴포넌트, MotionController, 디버그 컴포넌트는 M1 2단계에서 추가한다.
 * 설계: docs/design/03-m1-design.md 3장
 */
UCLASS()
class XRFPS_API AXRFPSPawn : public APawn
{
	GENERATED_BODY()

public:
	AXRFPSPawn();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void NotifyControllerChanged() override;

protected:
	/** 플레이 공간 바닥 기준점. 모든 포즈가 이 기준의 로컬 좌표다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR")
	TObjectPtr<USceneComponent> VROrigin;

	/** 머리. 헤드셋이 있으면 엔진이 HMD에 붙여 움직이고, 없으면 Tick에서 입력 프레임의 머리 포즈를 적용한다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR")
	TObjectPtr<UXRPoseComponent> Pose;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR")
	TObjectPtr<UXRDesktopInputSource> DesktopInput;
};
