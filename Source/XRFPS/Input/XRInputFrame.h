// Copyright XRFPS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "XRInputFrame.generated.h"

/**
 * 한 프레임 분량의 플레이어 입력.
 *
 * Quest(OpenXR)에서 오든 데스크톱 시뮬레이션에서 오든 같은 모양으로 만든다.
 * 이 구조체를 쓰는 쪽(손, 총, M2의 서버)은 입력이 어디서 왔는지 몰라도 된다.
 *
 * - 모든 포즈는 VROrigin 기준 로컬 좌표(cm)다. X 앞, Y 오른쪽, Z 위.
 * - 버튼·스틱은 기기에서 들어온 그대로(왼손/오른손)를 담는다.
 *   "주 손의 트리거"처럼 해석하는 일은 PrimaryHand를 아는 쪽이 한다.
 *
 * 설계: docs/design/03-m1-design.md 4장
 */
USTRUCT(BlueprintType)
struct FXRInputFrame
{
	GENERATED_BODY()

	/** 입력을 만든 순서. M2에서 서버가 순서를 맞출 때 쓴다. */
	UPROPERTY(BlueprintReadOnly, Category = "XR|Input")
	int32 FrameNumber = 0;

	/** 입력을 만든 시각(월드 시간, 초). M2 지연 보정에 쓴다. */
	UPROPERTY(BlueprintReadOnly, Category = "XR|Input")
	double TimeSeconds = 0.0;

	// --- 포즈 ---

	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Pose")
	FTransform Head = FTransform::Identity;

	/** 손으로 쥔 중심. 총을 붙이는 기준이다. */
	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Pose")
	FTransform LeftGrip = FTransform::Identity;

	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Pose")
	FTransform RightGrip = FTransform::Identity;

	/** 가리키는 방향. UI 전용이라 M2에서 서버로 보내지 않는다. */
	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Pose")
	FTransform LeftAim = FTransform::Identity;

	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Pose")
	FTransform RightAim = FTransform::Identity;

	// --- 트래킹 상태 ---

	/** false인 구간은 조준 안정도 계산과 치트 검증에서 제외한다. */
	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Tracking")
	bool bHeadTracked = false;

	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Tracking")
	bool bLeftHandTracked = false;

	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Tracking")
	bool bRightHandTracked = false;

	/** 헤드셋을 벗으면 false. 자리 비움 처리에 쓴다. */
	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Tracking")
	bool bHMDWorn = false;

	// --- 버튼·스틱 (기기 그대로) ---

	/** 트리거 0~1 */
	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Buttons")
	float LeftTrigger = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Buttons")
	float RightTrigger = 0.f;

	/** 썸스틱 -1~1 */
	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Buttons")
	FVector2D LeftThumbstick = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "XR|Input|Buttons")
	FVector2D RightThumbstick = FVector2D::ZeroVector;
};
