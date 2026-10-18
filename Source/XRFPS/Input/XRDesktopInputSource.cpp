// Copyright XRFPS. All Rights Reserved.

#include "Input/XRDesktopInputSource.h"

#include "Engine/Engine.h"
#include "EnhancedInputComponent.h"
#include "IXRTrackingSystem.h"
#include "Input/XRInputFrame.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

bool UXRDesktopInputSource::IsAvailable() const
{
	// 헤드셋 트래킹이 켜져 있지 않으면 데스크톱 시뮬레이션을 쓴다.
	// (UHeadMountedDisplayFunctionLibrary는 5.8에서 XRBase 플러그인으로 옮겨져, 엔진 기본 모듈의 IXRTrackingSystem을 직접 본다)
	UWorld* World = GetWorld();
	const bool bXRActive = GEngine && GEngine->XRSystem.IsValid() && World && GEngine->XRSystem->IsHeadTrackingAllowedForWorld(*World);
	return !bXRActive;
}

UInputMappingContext* UXRDesktopInputSource::GetMappingContext()
{
	CreateInput();
	return MappingContext;
}

void UXRDesktopInputSource::CreateInput()
{
	if (MappingContext)
	{
		return;
	}

	// 데스크톱 시뮬레이션은 디버그 도구라 에셋 대신 코드로 입력을 만든다.
	// 그래서 키 배치가 바뀌어도 diff로 바로 보인다.
	LookAction = NewObject<UInputAction>(this, TEXT("IA_Desktop_Look"));
	LookAction->ValueType = EInputActionValueType::Axis2D;

	MappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Desktop"));

	// 마우스를 위로 밀면 Y가 +, 우리 HeadPitch도 위가 +라서 뒤집지 않는다.
	MappingContext->MapKey(LookAction, EKeys::Mouse2D);

	// [과제] 프리셋(1, 2 키), 손 미세 조정, 트리거(마우스 왼쪽 버튼) 액션을 만들고 여기서 키에 연결
}

void UXRDesktopInputSource::BindInput(UEnhancedInputComponent* EnhancedInput)
{
	if (!EnhancedInput)
	{
		return;
	}

	CreateInput();

	// Triggered: 마우스가 움직이는 동안 매 프레임 호출
	EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &UXRDesktopInputSource::Look);

	// [과제] 위에서 만든 액션들을 함수에 연결
}

void UXRDesktopInputSource::Look(const FInputActionValue& Value)
{
	const FVector2D Delta = Value.Get<FVector2D>();

	HeadYaw += Delta.X * LookSensitivity;
	HeadPitch = FMath::Clamp(HeadPitch + Delta.Y * LookSensitivity, -MaxHeadPitch, MaxHeadPitch);
}

void UXRDesktopInputSource::BuildFrame(FXRInputFrame& OutFrame)
{
	// 머리: 바닥(VROrigin)에서 StandingHeadHeight 위, 마우스로 돌린 방향
	const FTransform HeadTransform(FRotator(HeadPitch, HeadYaw, 0.f), FVector(0.f, 0.f, StandingHeadHeight));
	OutFrame.Head = HeadTransform;

	// 손: 머리 기준 오프셋을 VROrigin 기준으로 바꾼다.
	// FTransform의 A * B는 "A를 B 기준으로 놓는다"는 뜻. 그래서 오프셋 * 머리 = 머리 기준 오프셋의 VROrigin 좌표
	OutFrame.RightGrip = FTransform(RightHandOffset) * HeadTransform;
	OutFrame.LeftGrip = FTransform(LeftHandOffset) * HeadTransform;

	// 데스크톱에서는 Aim을 Grip과 같게 둔다. Aim은 UI 전용이라 M1에서는 쓰지 않는다.
	OutFrame.RightAim = OutFrame.RightGrip;
	OutFrame.LeftAim = OutFrame.LeftGrip;

	// 시뮬레이션은 항상 "잘 추적되고, 헤드셋을 쓰고 있는" 상태
	OutFrame.bHeadTracked = true;
	OutFrame.bLeftHandTracked = true;
	OutFrame.bRightHandTracked = true;
	OutFrame.bHMDWorn = true;

	// [과제] 트리거 값을 OutFrame.RightTrigger에 채우기
}
