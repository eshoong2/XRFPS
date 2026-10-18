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
 * - 1 / 2: 왼손 자세 프리셋 (한 손 들기 / 양손 파지)
 * - I K / J L / U O: 왼손 미세 조정 (앞뒤 / 좌우 / 위아래). 양손 파지 판정 경계 테스트용
 * - 마우스 왼쪽 버튼: 오른손 트리거
 *
 * 헤드셋이 켜져 있지 않을 때만 IsAvailable()이 true다.
 */
UCLASS(ClassGroup = (XR), meta = (BlueprintSpawnableComponent))
class XRFPS_API UXRDesktopInputSource : public UXRInputSource
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
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

	/** 프리셋 1(한 손 들기): 머리 기준 왼손 Grip 위치(cm). 왼손은 몸 옆으로 내림 */
	UPROPERTY(EditAnywhere, Category = "XR|Desktop|Preset")
	FVector OneHandLeftOffset = FVector(5.f, -25.f, -60.f);

	/** 프리셋 2(양손 파지): 머리 기준 왼손 Grip 위치(cm). 오른손 앞 약 25cm, 총열 아래쪽 */
	UPROPERTY(EditAnywhere, Category = "XR|Desktop|Preset")
	FVector TwoHandLeftOffset = FVector(60.f, 16.f, -33.f);

	/** 미세 조정 키를 누르고 있을 때 왼손이 움직이는 속도(cm/초) */
	UPROPERTY(EditAnywhere, Category = "XR|Desktop|Adjust", meta = (ClampMin = "1"))
	float LeftHandAdjustSpeed = 10.f;

private:
	/** 입력 액션과 매핑을 코드로 만든다. 처음 필요할 때 한 번만. */
	void CreateInput();

	void Look(const FInputActionValue& Value);

	/** 1, 2 키: 왼손을 프리셋 위치로 옮긴다. PresetIndex는 BindAction에서 키마다 다르게 넘겨준다. */
	void ApplyLeftHandPreset(int32 PresetIndex);

	/** 미세 조정 키를 누르고 있는 동안 매 프레임: 왼손을 조금씩 옮긴다. */
	void AdjustLeftHand(const FInputActionValue& Value);

	void PressTrigger();
	void ReleaseTrigger();

	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> PresetOneHandAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> PresetTwoHandAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> AdjustLeftHandAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> TriggerAction;

	float HeadYaw = 0.f;
	float HeadPitch = 0.f;

	/** 지금 왼손 위치(머리 기준). 프리셋과 미세 조정이 바꾼다. 에디터 Details에서 실행 중 값을 볼 수 있다. */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "XR|Desktop")
	FVector CurrentLeftHandOffset = FVector::ZeroVector;

	bool bTriggerHeld = false;
};
