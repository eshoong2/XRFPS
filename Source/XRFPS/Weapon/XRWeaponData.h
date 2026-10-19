// Copyright XRFPS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "XRWeaponData.generated.h"

/**
 * 무기별 기준값. 코드는 이 값을 읽어 판정만 한다. (데이터 주도 설계)
 *
 * 숫자는 모두 초기값이고 플레이테스트로 조정한다. 무기를 추가할 때는 코드 대신 이 에셋을 새로 만든다.
 * 설계: docs/design/03-m1-design.md 6장
 */
UCLASS(BlueprintType)
class XRFPS_API UXRWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FText DisplayName = FText::FromString(TEXT("Rifle"));

	/** 히트스캔 사거리(cm) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Fire", meta = (ClampMin = "100"))
	float FireRange = 10000.f;

	// --- 양손 파지 판정 (M1 5단계에서 사용) ---

	/** 보조 손 Grip이 총열 선에서 이 거리(cm) 안으로 들어오면 양손 파지가 켜진다 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|TwoHand", meta = (ClampMin = "0"))
	float SupportEnterDistance = 5.f;

	/** 양손 파지 중에는 이 거리(cm) 밖으로 나가야 꺼진다. Enter보다 커야 경계에서 깜빡이지 않는다 (히스테리시스) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|TwoHand", meta = (ClampMin = "0"))
	float SupportExitDistance = 7.f;

	/** 두 손 사이 거리 허용 범위(cm) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|TwoHand", meta = (ClampMin = "0"))
	float MinHandDistance = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|TwoHand", meta = (ClampMin = "0"))
	float MaxHandDistance = 30.f;
};
