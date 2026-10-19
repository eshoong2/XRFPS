// Copyright XRFPS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XRHandComponent.generated.h"

struct FXRInputFrame;
class UMotionControllerComponent;

/** 왼손 / 오른손 */
UENUM(BlueprintType)
enum class EXRHandSide : uint8
{
	Left,
	Right
};

/**
 * 손 하나의 상태를 묶는 컴포넌트. Pawn에 왼손용, 오른손용 두 개가 붙는다.
 *
 * - 이 손이 따라가는 MotionController(Grip, Aim)를 가리킨다
 * - 매 프레임 입력 프레임에서 "내 손" 몫(트래킹, 트리거, 스틱, Grip 포즈)만 꺼내 보관한다
 *
 * 템플릿처럼 왼손·오른손 변수를 쌍으로 두지 않고, 같은 코드의 손 두 개를 둔다.
 * 그래서 "주 손"은 이 컴포넌트 둘 중 하나를 가리키기만 하면 된다. (ADR-0004 결정 1)
 */
UCLASS(ClassGroup = (XR), meta = (BlueprintSpawnableComponent))
class XRFPS_API UXRHandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UXRHandComponent();

	/** Pawn 생성자에서 호출한다. 이 손이 어느 쪽인지, 어떤 MotionController를 따라가는지 연결한다. */
	void Initialize(EXRHandSide InSide, UMotionControllerComponent* InGripController, UMotionControllerComponent* InAimController);

	/** 이번 프레임 입력에서 이 손의 몫을 읽는다. Pawn Tick에서 호출한다. */
	void UpdateFromFrame(const FXRInputFrame& Frame);

	EXRHandSide GetSide() const { return Side; }
	bool IsTracked() const { return bTracked; }
	float GetTriggerValue() const { return TriggerValue; }
	FVector2D GetThumbstick() const { return Thumbstick; }

	/** VROrigin 기준 Grip 포즈. M2에서 서버가 같은 값으로 다시 계산하므로, 판정 로직은 이 값을 쓴다. */
	const FTransform& GetLocalGripTransform() const { return LocalGrip; }

	/** 화면에 보이는 손(Grip MotionController)의 월드 Transform. 총을 붙이거나 그릴 때 쓴다. */
	FTransform GetWorldGripTransform() const;

	UMotionControllerComponent* GetGripController() const { return GripController; }
	UMotionControllerComponent* GetAimController() const { return AimController; }

private:
	UPROPERTY(VisibleAnywhere, Category = "XR|Hand")
	EXRHandSide Side = EXRHandSide::Right;

	UPROPERTY(Transient)
	TObjectPtr<UMotionControllerComponent> GripController;

	UPROPERTY(Transient)
	TObjectPtr<UMotionControllerComponent> AimController;

	FTransform LocalGrip = FTransform::Identity;
	bool bTracked = false;
	float TriggerValue = 0.f;
	FVector2D Thumbstick = FVector2D::ZeroVector;
};
