// Copyright XRFPS. All Rights Reserved.

#include "Player/XRFPSPawn.h"

#include "Camera/CameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Input/XRDesktopInputSource.h"
#include "Input/XRInputSource.h"
#include "Player/XRPoseComponent.h"

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

	Pose = CreateDefaultSubobject<UXRPoseComponent>(TEXT("Pose"));
	DesktopInput = CreateDefaultSubobject<UXRDesktopInputSource>(TEXT("DesktopInput"));
}

void AXRFPSPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 1. 입력 갱신이 항상 먼저. 아래 단계는 모두 이번 프레임 입력을 읽는다.
	Pose->UpdateFrame();

	// 2. 머리: 데스크톱 입력일 때만 카메라를 직접 옮긴다. 헤드셋이면 엔진이 HMD를 따라 움직인다(bLockToHmd).
	if (Pose->GetActiveSource() == DesktopInput)
	{
		Camera->SetRelativeTransform(Pose->GetCurrentFrame().Head);
	}

	// (M1 2단계) 3. 손, (3단계) 4. 총
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
