// Copyright XRFPS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Player/XRHandComponent.h"
#include "XRFPSPawn.generated.h"

class AXRWeapon;
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
 * 총(AXRWeapon)은 별도 Actor로, BeginPlay에서 만들어 주 손 Grip에 Snap 장착한다.
 *
 * Tick 순서: 입력 → (필요하면) 컴포넌트에 포즈 적용 → 손 → 총 → 디버그
 */
UCLASS()
class XRFPS_API AXRFPSPawn : public APawn
{
	GENERATED_BODY()

public:
	AXRFPSPawn();

	virtual void BeginPlay() override;
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

	/**
	 * 무기를 주 손에 Snap으로 장착한다. 이미 들고 있던 무기는 제거한다.
	 * 라운드 시작 시 기본 무기에 쓰고, 나중에 무기 교체(백로그)도 이 함수를 재사용한다.
	 */
	void EquipWeapon(AXRWeapon* Weapon);

	/** 주 손이 쥔 무기. 없으면 nullptr */
	AXRWeapon* GetEquippedWeapon() const;

protected:
	/**
	 * 시작할 때 주 손에 장착할 무기 클래스. M1은 Pawn이 BeginPlay에서 직접 만든다.
	 * (M2: 서버가 라운드 시작 때 만들어 장착한다)
	 */
	UPROPERTY(EditAnywhere, Category = "XR|Weapon")
	TSubclassOf<AXRWeapon> DefaultWeaponClass;

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
