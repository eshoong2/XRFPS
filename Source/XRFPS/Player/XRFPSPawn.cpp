// Copyright XRFPS. All Rights Reserved.

#include "Player/XRFPSPawn.h"

#include "Camera/CameraComponent.h"
#include "Debug/XRDebugDrawComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Input/XRDesktopInputSource.h"
#include "Input/XRInputFrame.h"
#include "Input/XRInputSource.h"
#include "MotionControllerComponent.h"
#include "Player/XRPoseComponent.h"

namespace XRFPSPawnNames
{
	// OpenXR 표준 포즈 이름. 엔진이 이 이름으로 실제 컨트롤러 포즈를 찾아 준다.
	const FName LeftGrip(TEXT("LeftGrip"));
	const FName LeftAim(TEXT("LeftAim"));
	const FName RightGrip(TEXT("RightGrip"));
	const FName RightAim(TEXT("RightAim"));
}

AXRFPSPawn::AXRFPSPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// 몸 방향은 입력 프레임(머리 포즈)이 정한다. 컨트롤러 회전은 따르지 않는다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	RootComponent = VROrigin;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VROrigin);
	Camera->bLockToHmd = true;

	auto CreateMotionController = [this](const TCHAR* Name, const FName& MotionSource)
	{
		UMotionControllerComponent* Controller = CreateDefaultSubobject<UMotionControllerComponent>(Name);
		Controller->SetupAttachment(VROrigin);
		Controller->SetTrackingMotionSource(MotionSource);
		return Controller;
	};

	MC_LeftGrip = CreateMotionController(TEXT("MC_LeftGrip"), XRFPSPawnNames::LeftGrip);
	MC_LeftAim = CreateMotionController(TEXT("MC_LeftAim"), XRFPSPawnNames::LeftAim);
	MC_RightGrip = CreateMotionController(TEXT("MC_RightGrip"), XRFPSPawnNames::RightGrip);
	MC_RightAim = CreateMotionController(TEXT("MC_RightAim"), XRFPSPawnNames::RightAim);

	LeftHand = CreateDefaultSubobject<UXRHandComponent>(TEXT("LeftHand"));
	LeftHand->Initialize(EXRHandSide::Left, MC_LeftGrip, MC_LeftAim);

	RightHand = CreateDefaultSubobject<UXRHandComponent>(TEXT("RightHand"));
	RightHand->Initialize(EXRHandSide::Right, MC_RightGrip, MC_RightAim);

	Pose = CreateDefaultSubobject<UXRPoseComponent>(TEXT("Pose"));
	DesktopInput = CreateDefaultSubobject<UXRDesktopInputSource>(TEXT("DesktopInput"));
	Debug = CreateDefaultSubobject<UXRDebugDrawComponent>(TEXT("Debug"));
}

UXRHandComponent* AXRFPSPawn::GetHand(EXRHandSide Side) const
{
	return (Side == EXRHandSide::Left) ? LeftHand : RightHand;
}

UXRHandComponent* AXRFPSPawn::GetSupportHand() const
{
	return GetHand(PrimaryHandSide == EXRHandSide::Left ? EXRHandSide::Right : EXRHandSide::Left);
}

void AXRFPSPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 1. 입력 갱신이 항상 먼저. 아래 단계는 모두 이번 프레임 입력을 읽는다.
	Pose->UpdateFrame();
	const FXRInputFrame& Frame = Pose->GetCurrentFrame();

	// 2. 화면에 보이는 머리·손 컴포넌트를 입력 위치로. 헤드셋이면 엔진이 이미 옮겼으므로 건너뛴다.
	const UXRInputSource* Source = Pose->GetActiveSource();
	if (Source && Source->ShouldApplyFrameToComponents())
	{
		ApplyFrameToComponents(Frame);
	}

	// 3. 손 상태
	LeftHand->UpdateFromFrame(Frame);
	RightHand->UpdateFromFrame(Frame);

	// (M1 3단계) 4. 총

	// 5. 디버그는 마지막. 이번 프레임 갱신이 모두 끝난 상태를 그린다.
	Debug->DrawDebug();
}

void AXRFPSPawn::ApplyFrameToComponents(const FXRInputFrame& Frame)
{
	// 프레임의 포즈는 VROrigin 기준이고, 이 컴포넌트들은 모두 VROrigin에 붙어 있으므로 상대 Transform으로 그대로 넣으면 된다.
	Camera->SetRelativeTransform(Frame.Head);
	MC_LeftGrip->SetRelativeTransform(Frame.LeftGrip);
	MC_LeftAim->SetRelativeTransform(Frame.LeftAim);
	MC_RightGrip->SetRelativeTransform(Frame.RightGrip);
	MC_RightAim->SetRelativeTransform(Frame.RightAim);
}

void AXRFPSPawn::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// 이 Pawn을 조종하는 플레이어의 입력 서브시스템에 입력 소스들의 매핑을 등록한다.
	const APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!Subsystem)
	{
		return;
	}

	TArray<UXRInputSource*> Sources;
	GetComponents(Sources);
	for (UXRInputSource* Source : Sources)
	{
		if (UInputMappingContext* MappingContext = Source->GetMappingContext())
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void AXRFPSPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		return;
	}

	// 붙어 있는 모든 입력 소스가 자기 액션을 연결한다. 어떤 소스를 쓸지는 Pose가 매 프레임 고른다.
	TArray<UXRInputSource*> Sources;
	GetComponents(Sources);
	for (UXRInputSource* Source : Sources)
	{
		Source->BindInput(EnhancedInput);
	}
}
