// Copyright XRFPS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/XRInputSource.h"
#include "XRDesktopInputSource.generated.h"

class UInputAction;
struct FInputActionValue;

/**
 * 헤드셋 없이 키보드·마우스로 머리와 양손을 흉내 내는 입력 소스. (ADR-0001)
 *
 * - 마우스: 머리 회전
 * - 양손: 머리 기준 오프셋에 붙어서 머리를 따라 움직인다 (총을 들고 고개를 돌리는 느낌)
 *
 * 헤드셋이 켜져 있지 않을 때만 IsAvailable()이 true다.
 */
UCLASS(ClassGroup = (XR), meta = (BlueprintSpawnableComponent))
class XRFPS_API UXRDesktopInputSource : public UXRInputSource
{
	GENERATED_BODY()

public:
	virtual bool IsAvailable() const override;
	virtual FString GetSourceName() const override { return TEXT("Desktop"); }
	virtual UInputMappingContext* GetMappingContext() override;
	virtual void BindInput(UEnhancedInputComponent* EnhancedInput) override;
	virtual void BuildFrame(FXRInputFrame& OutFrame) override;

protected:
	/** 서 있을 때 VROrigin(바닥)에서 머리까지 높이(cm) */
	UPROPERTY(EditAnywhere, Category = "XR|Desktop", meta = (ClampMin = "100", ClampMax = "220"))
	float StandingHeadHeight = 170.f;

	/** 마우스 1 단위당 머리가 도는 각도(도) */
	UPROPERTY(EditAnywhere, Category = "XR|Desktop", meta = (ClampMin = "0.01"))
	float LookSensitivity = 1.f;

	/** 위아래로 고개를 숙이고 젖히는 최대 각도(도) */
	UPROPERTY(EditAnywhere, Category = "XR|Desktop", meta = (ClampMin = "0", ClampMax = "89"))
	float MaxHeadPitch = 85.f;

	/** 머리 기준 오른손 Grip 위치(cm). X 앞, Y 오른쪽, Z 위 */
	UPROPERTY(EditAnywhere, Category = "XR|Desktop")
	FVector RightHandOffset = FVector(35.f, 18.f, -30.f);

	/** 머리 기준 왼손 Grip 위치(cm) */
	UPROPERTY(EditAnywhere, Category = "XR|Desktop")
	FVector LeftHandOffset = FVector(5.f, -25.f, -60.f);

	// [과제] 손 자세 프리셋(1 = 한 손 들기, 2 = 양손 파지 자세)과 미세 조정 값을 여기에 추가

private:
	/** 입력 액션과 매핑을 코드로 만든다. 처음 필요할 때 한 번만. */
	void CreateInput();

	void Look(const FInputActionValue& Value);

	// [과제] 프리셋 선택, 손 미세 조정, 트리거 입력 함수를 여기에 추가

	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> LookAction;

	float HeadYaw = 0.f;
	float HeadPitch = 0.f;
};
