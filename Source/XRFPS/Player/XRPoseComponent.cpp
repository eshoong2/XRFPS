// Copyright XRFPS. All Rights Reserved.

#include "Player/XRPoseComponent.h"

#include "GameFramework/Actor.h"
#include "Input/XRInputSource.h"
#include "XRFPS.h"

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
}
