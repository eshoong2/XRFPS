// Copyright XRFPS. All Rights Reserved.

#include "Player/XRHandComponent.h"

#include "Input/XRInputFrame.h"
#include "MotionControllerComponent.h"

UXRHandComponent::UXRHandComponent()
{
	// 손은 스스로 Tick하지 않는다. Pawn이 입력 갱신 직후 UpdateFromFrame()을 호출한다. (Pose 컴포넌트와 같은 이유)
	PrimaryComponentTick.bCanEverTick = false;
}

void UXRHandComponent::Initialize(EXRHandSide InSide, UMotionControllerComponent* InGripController, UMotionControllerComponent* InAimController)
{
	Side = InSide;
	GripController = InGripController;
	AimController = InAimController;
}

void UXRHandComponent::UpdateFromFrame(const FXRInputFrame& Frame)
{
	// 입력 프레임은 왼손·오른손 값을 모두 담고 있다. 여기서 "내 쪽" 값만 꺼낸다.
	// 이렇게 한 곳에서 갈라 두면, 이 아래 코드는 손이 어느 쪽인지 신경 쓰지 않아도 된다.
	const bool bIsLeft = (Side == EXRHandSide::Left);

	LocalGrip = bIsLeft ? Frame.LeftGrip : Frame.RightGrip;
	bTracked = bIsLeft ? Frame.bLeftHandTracked : Frame.bRightHandTracked;
	TriggerValue = bIsLeft ? Frame.LeftTrigger : Frame.RightTrigger;
	Thumbstick = bIsLeft ? Frame.LeftThumbstick : Frame.RightThumbstick;
}

FTransform UXRHandComponent::GetWorldGripTransform() const
{
	return GripController ? GripController->GetComponentTransform() : FTransform::Identity;
}
