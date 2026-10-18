// Copyright XRFPS. All Rights Reserved.

#include "Input/XRDesktopInputSource.h"

#include "Engine/Engine.h"
#include "EnhancedInputComponent.h"
#include "IXRTrackingSystem.h"
#include "Input/XRInputFrame.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"

namespace XRDesktopInput
{
	constexpr int32 PresetOneHand = 1;
	constexpr int32 PresetTwoHand = 2;
}

void UXRDesktopInputSource::BeginPlay()
{
	Super::BeginPlay();

	// 시작 자세는 한 손 들기
	ApplyLeftHandPreset(XRDesktopInput::PresetOneHand);
}

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
	auto MakeAction = [this](const TCHAR* Name, EInputActionValueType ValueType)
	{
		UInputAction* Action = NewObject<UInputAction>(this, Name);
		Action->ValueType = ValueType;
		return Action;
	};

	LookAction = MakeAction(TEXT("IA_Desktop_Look"), EInputActionValueType::Axis2D);
	PresetOneHandAction = MakeAction(TEXT("IA_Desktop_PresetOneHand"), EInputActionValueType::Boolean);
	PresetTwoHandAction = MakeAction(TEXT("IA_Desktop_PresetTwoHand"), EInputActionValueType::Boolean);
	AdjustLeftHandAction = MakeAction(TEXT("IA_Desktop_AdjustLeftHand"), EInputActionValueType::Axis3D);
	TriggerAction = MakeAction(TEXT("IA_Desktop_Trigger"), EInputActionValueType::Boolean);

	// 미세 조정은 키 여러 개가 한 액션의 X·Y·Z를 나눠 맡는다.
	// 기본값(TakeHighestAbsoluteValue)이면 동시에 누른 키 중 하나만 반영되므로, 값을 더하도록 바꾼다.
	// 그러면 I+L을 같이 누르면 대각선으로 움직이고, I+K는 서로 상쇄된다.
	AdjustLeftHandAction->AccumulationBehavior = EInputActionAccumulationBehavior::Cumulative;

	MappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Desktop"));

	// 마우스를 위로 밀면 Y가 +, 우리 HeadPitch도 위가 +라서 뒤집지 않는다.
	MappingContext->MapKey(LookAction, EKeys::Mouse2D);

	MappingContext->MapKey(PresetOneHandAction, EKeys::One);
	MappingContext->MapKey(PresetTwoHandAction, EKeys::Two);
	MappingContext->MapKey(TriggerAction, EKeys::LeftMouseButton);

	// 키 하나는 X축 값(1.0)만 낸다. Swizzle로 그 값을 Y나 Z 자리로 옮기고, Negate로 부호를 뒤집는다.
	// 축은 머리 기준: X 앞, Y 오른쪽, Z 위
	auto MapAdjustKey = [this](const FKey& Key, EInputAxisSwizzle Swizzle, bool bUseSwizzle, bool bNegate)
	{
		FEnhancedActionKeyMapping& Mapping = MappingContext->MapKey(AdjustLeftHandAction, Key);
		if (bUseSwizzle)
		{
			UInputModifierSwizzleAxis* SwizzleModifier = NewObject<UInputModifierSwizzleAxis>(this);
			SwizzleModifier->Order = Swizzle;
			Mapping.Modifiers.Add(SwizzleModifier);
		}
		if (bNegate)
		{
			Mapping.Modifiers.Add(NewObject<UInputModifierNegate>(this));
		}
	};

	MapAdjustKey(EKeys::I, EInputAxisSwizzle::YXZ, false, false);	// 앞 (+X)
	MapAdjustKey(EKeys::K, EInputAxisSwizzle::YXZ, false, true);	// 뒤 (-X)
	MapAdjustKey(EKeys::L, EInputAxisSwizzle::YXZ, true, false);	// 오른쪽 (+Y): X 값을 Y로
	MapAdjustKey(EKeys::J, EInputAxisSwizzle::YXZ, true, true);		// 왼쪽 (-Y)
	MapAdjustKey(EKeys::O, EInputAxisSwizzle::ZYX, true, false);	// 위 (+Z): X 값을 Z로
	MapAdjustKey(EKeys::U, EInputAxisSwizzle::ZYX, true, true);		// 아래 (-Z)
}

void UXRDesktopInputSource::BindInput(UEnhancedInputComponent* EnhancedInput)
{
	if (!EnhancedInput)
	{
		return;
	}

	CreateInput();

	// Triggered: 누르고 있는(움직이는) 동안 매 프레임 / Started: 누른 순간 한 번 / Completed: 뗀 순간 한 번
	EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &UXRDesktopInputSource::Look);

	// 마지막 인자(프리셋 번호)가 ApplyLeftHandPreset(int32)에 그대로 전달된다. 함수 하나로 키 두 개를 처리한다.
	EnhancedInput->BindAction(PresetOneHandAction, ETriggerEvent::Started, this, &UXRDesktopInputSource::ApplyLeftHandPreset, XRDesktopInput::PresetOneHand);
	EnhancedInput->BindAction(PresetTwoHandAction, ETriggerEvent::Started, this, &UXRDesktopInputSource::ApplyLeftHandPreset, XRDesktopInput::PresetTwoHand);

	EnhancedInput->BindAction(AdjustLeftHandAction, ETriggerEvent::Triggered, this, &UXRDesktopInputSource::AdjustLeftHand);

	EnhancedInput->BindAction(TriggerAction, ETriggerEvent::Started, this, &UXRDesktopInputSource::PressTrigger);
	EnhancedInput->BindAction(TriggerAction, ETriggerEvent::Completed, this, &UXRDesktopInputSource::ReleaseTrigger);
}

void UXRDesktopInputSource::Look(const FInputActionValue& Value)
{
	const FVector2D Delta = Value.Get<FVector2D>();

	HeadYaw += Delta.X * LookSensitivity;
	HeadPitch = FMath::Clamp(HeadPitch + Delta.Y * LookSensitivity, -MaxHeadPitch, MaxHeadPitch);
}

void UXRDesktopInputSource::ApplyLeftHandPreset(int32 PresetIndex)
{
	CurrentLeftHandOffset = (PresetIndex == XRDesktopInput::PresetTwoHand) ? TwoHandLeftOffset : OneHandLeftOffset;
}

void UXRDesktopInputSource::AdjustLeftHand(const FInputActionValue& Value)
{
	// 키를 누르는 동안 매 프레임 호출된다. 프레임 시간을 곱해야 프레임레이트와 상관없이 같은 속도로 움직인다.
	const float DeltaSeconds = GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.f;
	CurrentLeftHandOffset += Value.Get<FVector>() * LeftHandAdjustSpeed * DeltaSeconds;
}

void UXRDesktopInputSource::PressTrigger()
{
	bTriggerHeld = true;
}

void UXRDesktopInputSource::ReleaseTrigger()
{
	bTriggerHeld = false;
}

void UXRDesktopInputSource::BuildFrame(FXRInputFrame& OutFrame)
{
	// 머리: 바닥(VROrigin)에서 StandingHeadHeight 위, 마우스로 돌린 방향
	const FTransform HeadTransform(FRotator(HeadPitch, HeadYaw, 0.f), FVector(0.f, 0.f, StandingHeadHeight));
	OutFrame.Head = HeadTransform;

	// 손: 머리 기준 오프셋을 VROrigin 기준으로 바꾼다.
	// FTransform의 A * B는 "A를 B 기준으로 놓는다"는 뜻. 그래서 오프셋 * 머리 = 머리 기준 오프셋의 VROrigin 좌표
	OutFrame.RightGrip = FTransform(RightHandOffset) * HeadTransform;
	OutFrame.LeftGrip = FTransform(CurrentLeftHandOffset) * HeadTransform;

	// 데스크톱에서는 Aim을 Grip과 같게 둔다. Aim은 UI 전용이라 M1에서는 쓰지 않는다.
	OutFrame.RightAim = OutFrame.RightGrip;
	OutFrame.LeftAim = OutFrame.LeftGrip;

	// 시뮬레이션은 항상 "잘 추적되고, 헤드셋을 쓰고 있는" 상태
	OutFrame.bHeadTracked = true;
	OutFrame.bLeftHandTracked = true;
	OutFrame.bRightHandTracked = true;
	OutFrame.bHMDWorn = true;

	// 마우스 왼쪽 버튼은 오른손 트리거로 흉내 낸다. (MVP의 주 손은 오른손)
	OutFrame.RightTrigger = bTriggerHeld ? 1.f : 0.f;
}
