// Copyright XRFPS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Player/XRHandComponent.h"
#include "XRFPSPawn.generated.h"

class UCameraComponent;
class UMotionControllerComponent;
class UXRDebugDrawComponent;
class UXRDesktopInputSource;
class UXRPoseComponent;
struct FXRInputFrame;

/**
 * XR 플레이어 Pawn. (docs/design/03-m1-design.md 3장, C안)
 *
 * AXRFPSPawn
 *  ├─ VROrigin                              플레이 공간 바닥 기준점
 *  │   ├─ Camera                            머리
 *  │   ├─ MC_LeftGrip / MC_LeftAim          왼손 포즈 2개
 *  │   └─ MC_RightGrip / MC_RightAim        오른손 포즈 2개
 *  ├─ LeftHand, RightHand                   손 하나의 상태
 *  ├─ Pose                                  이번 프레임 입력
 *  ├─ DesktopInput                          키보드·마우스 입력 소스
 *  └─ Debug                                 디버그 표시
 *
 * Tick 순서: 입력 → (필요하면) 컴포넌트에 포즈 적용 → 손 → (M1 3단계) 총 → 디버그
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

	UCameraComponent* GetCamera() const { return Camera; }
	UXRPoseComponent* GetPose() const { return Pose; }

	UXRHandComponent* GetHand(EXRHandSide Side) const;

	/** 총을 쥐는 손. 코드는 "오른손" 대신 항상 이 함수를 쓴다. (왼손잡이 지원은 백로그) */
	UXRHandComponent* GetPrimaryHand() const { return GetHand(PrimaryHandSide); }

	/** 총열을 받치는 손 */
	UXRHandComponent* GetSupportHand() const;

protected:
	/** 주 손. MVP는 오른손 고정이지만, 하드코딩 대신 이 값 하나로 정한다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR")
	EXRHandSide PrimaryHandSide = EXRHandSide::Right;

	/** 플레이 공간 바닥 기준점. 모든 포즈가 이 기준의 로컬 좌표다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR")
	TObjectPtr<USceneComponent> VROrigin;

	/** 머리. 헤드셋이 있으면 엔진이 HMD에 붙여 움직이고, 없으면 Tick에서 입력 프레임의 머리 포즈를 적용한다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR|Hands")
	TObjectPtr<UMotionControllerComponent> MC_LeftGrip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR|Hands")
	TObjectPtr<UMotionControllerComponent> MC_LeftAim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR|Hands")
	TObjectPtr<UMotionControllerComponent> MC_RightGrip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR|Hands")
	TObjectPtr<UMotionControllerComponent> MC_RightAim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR|Hands")
	TObjectPtr<UXRHandComponent> LeftHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR|Hands")
	TObjectPtr<UXRHandComponent> RightHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR")
	TObjectPtr<UXRPoseComponent> Pose;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR")
	TObjectPtr<UXRDesktopInputSource> DesktopInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR")
	TObjectPtr<UXRDebugDrawComponent> Debug;

private:
	/** 엔진이 움직여 주지 않는 입력 소스일 때, 프레임의 포즈를 Camera·MotionController에 직접 적용한다. */
	void ApplyFrameToComponents(const FXRInputFrame& Frame);
};
