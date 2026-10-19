// Copyright XRFPS. All Rights Reserved.

#include "Player/XRPoseComponent.h"

#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "Input/XRInputSource.h"
#include "XRFPS.h"

namespace XRPoseDebug
{
	static TAutoConsoleVariable<bool> CVarDrawPoses(
		TEXT("xr.Debug.DrawPoses"),
		true,
		TEXT("머리·양손 포즈 축과 입력 상태를 화면에 그린다."),
		ECVF_Cheat);
}

UXRPoseComponent::UXRPoseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UXRInputSource* UXRPoseComponent::SelectAvailableSource() const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	TArray<UXRInputSource*> Sources;
	Owner->GetComponents(Sources);

	for (UXRInputSource* Source : Sources)
	{
		if (Source && Source->IsAvailable())
		{
			return Source;
		}
	}

	return nullptr;
}

void UXRPoseComponent::UpdateFrame()
{
	// 헤드셋을 켜고 끄는 상황을 고려해 매 프레임 다시 고른다. 소스 수가 적어 비용은 무시할 만하다.
	UXRInputSource* Selected = SelectAvailableSource();
	if (Selected != ActiveSource)
	{
		UE_LOG(LogXRFPS, Log, TEXT("입력 소스 변경: %s -> %s"),
			ActiveSource ? *ActiveSource->GetSourceName() : TEXT("None"),
			Selected ? *Selected->GetSourceName() : TEXT("None"));
		ActiveSource = Selected;
	}

	if (!ActiveSource)
	{
		return;
	}

	FXRInputFrame NewFrame;
	ActiveSource->BuildFrame(NewFrame);

	// 순서와 시각은 입력 소스가 아니라 여기서 채운다. 어떤 소스든 같은 규칙을 따르게 하기 위해서다.
	NewFrame.FrameNumber = NextFrameNumber++;
	NewFrame.TimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

	CurrentFrame = NewFrame;

	if (XRPoseDebug::CVarDrawPoses.GetValueOnGameThread())
	{
		DrawDebugPoses();
	}
}

void UXRPoseComponent::DrawDebugPoses() const
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
	{
		return;
	}

	// 프레임의 포즈는 VROrigin 기준이다. Pawn의 루트(VROrigin)를 곱해 월드 좌표로 바꾼다.
	const FTransform OriginToWorld = Owner->GetActorTransform();

	auto DrawAxes = [World, &OriginToWorld](const FTransform& LocalPose, float Scale)
	{
		const FTransform WorldPose = LocalPose * OriginToWorld;
		DrawDebugCoordinateSystem(World, WorldPose.GetLocation(), WorldPose.Rotator(), Scale, false, -1.f, 0, 0.5f);
	};

	// 데스크톱에서는 카메라가 머리 위치에 있어 머리 축은 안 보인다. 손 축이 잘 보이게 크게 그린다.
	DrawAxes(CurrentFrame.Head, 10.f);
	DrawAxes(CurrentFrame.LeftGrip, 8.f);
	DrawAxes(CurrentFrame.RightGrip, 8.f);

	if (GEngine)
	{
		const FString Status = FString::Printf(
			TEXT("Input: %s | Frame %d | Trigger L %.2f R %.2f | Tracked H%d L%d R%d"),
			ActiveSource ? *ActiveSource->GetSourceName() : TEXT("None"),
			CurrentFrame.FrameNumber,
			CurrentFrame.LeftTrigger,
			CurrentFrame.RightTrigger,
			CurrentFrame.bHeadTracked ? 1 : 0,
			CurrentFrame.bLeftHandTracked ? 1 : 0,
			CurrentFrame.bRightHandTracked ? 1 : 0);

		// Key를 고정하면 매 프레임 새 줄이 쌓이지 않고 같은 줄이 갱신된다.
		GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetUniqueID()), 0.f, FColor::Cyan, Status);
	}
}
