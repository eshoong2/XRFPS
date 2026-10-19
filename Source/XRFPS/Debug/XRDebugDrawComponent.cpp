// Copyright XRFPS. All Rights Reserved.

#include "Debug/XRDebugDrawComponent.h"

#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "HAL/IConsoleManager.h"
#include "Input/XRInputSource.h"
#include "MotionControllerComponent.h"
#include "Player/XRFPSPawn.h"
#include "Player/XRHandComponent.h"
#include "Player/XRPoseComponent.h"
#include "Weapon/XRWeapon.h"
#include "Weapon/XRWeaponData.h"

namespace XRDebugDraw
{
	static TAutoConsoleVariable<bool> CVarDraw(
		TEXT("xr.Debug.Draw"),
		true,
		TEXT("머리·양손 축과 입력 상태줄 등 XR 디버그 표시를 켜고 끈다."),
		ECVF_Cheat);

	constexpr float GripAxisSize = 8.f;
	constexpr float AimAxisSize = 4.f;
	constexpr float HeadAxisSize = 10.f;
	constexpr float MuzzleAxisSize = 5.f;
	constexpr float BarrelLineLength = 150.f;
}

UXRDebugDrawComponent::UXRDebugDrawComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UXRDebugDrawComponent::DrawDebug() const
{
	if (!XRDebugDraw::CVarDraw.GetValueOnGameThread())
	{
		return;
	}

	const AXRFPSPawn* Pawn = Cast<AXRFPSPawn>(GetOwner());
	UWorld* World = GetWorld();
	if (!Pawn || !World)
	{
		return;
	}

	auto DrawAxes = [World](const FTransform& WorldTransform, float Size, float Thickness)
	{
		DrawDebugCoordinateSystem(World, WorldTransform.GetLocation(), WorldTransform.Rotator(), Size, false, -1.f, 0, Thickness);
	};

	// 머리: 데스크톱에서는 카메라가 머리 위치라 보이지 않는다. Quest 화면 녹화나 다른 플레이어 시점에서 의미가 있다.
	if (const UCameraComponent* Camera = Pawn->GetCamera())
	{
		DrawAxes(Camera->GetComponentTransform(), XRDebugDraw::HeadAxisSize, 0.5f);
	}

	// 손: Grip은 크고 두껍게, Aim은 작고 얇게. 둘이 다른 위치·방향이라는 걸 눈으로 확인하기 위해
	const EXRHandSide Sides[] = { EXRHandSide::Left, EXRHandSide::Right };
	for (const EXRHandSide Side : Sides)
	{
		const UXRHandComponent* Hand = Pawn->GetHand(Side);
		if (!Hand)
		{
			continue;
		}

		if (const UMotionControllerComponent* Grip = Hand->GetGripController())
		{
			DrawAxes(Grip->GetComponentTransform(), XRDebugDraw::GripAxisSize, 0.6f);
		}
		if (const UMotionControllerComponent* Aim = Hand->GetAimController())
		{
			DrawAxes(Aim->GetComponentTransform(), XRDebugDraw::AimAxisSize, 0.2f);
		}
	}

	// 총구: 축과, 총구 방향(X)으로 뻗는 노란 선. 이 선이 M1 4단계 사격 방향, 5단계 양손 파지 기준선(총열 선)이 된다
	const AXRWeapon* Weapon = Pawn->GetEquippedWeapon();
	if (Weapon)
	{
		const FTransform MuzzleTransform = Weapon->GetMuzzleTransform();
		DrawAxes(MuzzleTransform, XRDebugDraw::MuzzleAxisSize, 0.4f);

		const FVector Start = MuzzleTransform.GetLocation();
		const FVector End = Start + MuzzleTransform.GetUnitAxis(EAxis::X) * XRDebugDraw::BarrelLineLength;
		DrawDebugLine(World, Start, End, FColor::Yellow, false, -1.f, 0, 0.3f);
	}

	if (!GEngine)
	{
		return;
	}

	const UXRPoseComponent* Pose = Pawn->GetPose();
	const UXRInputSource* Source = Pose ? Pose->GetActiveSource() : nullptr;
	const UXRHandComponent* Primary = Pawn->GetPrimaryHand();
	const UXRHandComponent* Left = Pawn->GetHand(EXRHandSide::Left);
	const UXRHandComponent* Right = Pawn->GetHand(EXRHandSide::Right);

	const FString Status = FString::Printf(
		TEXT("Input: %s | Frame %d | Primary: %s | Weapon: %s | Trigger %.2f | Tracked L%d R%d"),
		Source ? *Source->GetSourceName() : TEXT("None"),
		Pose ? Pose->GetCurrentFrame().FrameNumber : -1,
		(Primary && Primary->GetSide() == EXRHandSide::Left) ? TEXT("Left") : TEXT("Right"),
		Weapon ? *Weapon->GetWeaponData()->DisplayName.ToString() : TEXT("None"),
		Primary ? Primary->GetTriggerValue() : 0.f,
		(Left && Left->IsTracked()) ? 1 : 0,
		(Right && Right->IsTracked()) ? 1 : 0);

	// Key를 고정하면 매 프레임 새 줄이 쌓이지 않고 같은 줄이 갱신된다.
	GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetUniqueID()), 0.f, FColor::Cyan, Status);
}
