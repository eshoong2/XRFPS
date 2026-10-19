// Copyright XRFPS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XRWeapon.generated.h"

class UStaticMeshComponent;
class UXRWeaponData;

/**
 * 총. Pawn과 별개의 Actor다. (ADR-0004: 총 종류 교체, 무기별 규칙, M2 동기화 단위)
 *
 * 좌표 기준: 액터 원점 = 손잡이를 쥔 손의 중심(Grip). X 앞(총구 방향), Y 오른쪽, Z 위.
 * 그래서 주 손 Grip MotionController에 원점을 그대로 맞춰 붙이면(Snap) 손에 쥔 모양이 된다.
 *
 * M1에서는 엔진 기본 도형으로 임시 모양을 만든다. 실제 모델은 나중에 교체.
 * 메시 소켓 대신 SceneComponent(Muzzle)로 총구 위치를 표시한다. 기본 도형 조합이라 소켓을 붙일 메시가 하나가 아니기 때문이다.
 */
UCLASS()
class XRFPS_API AXRWeapon : public AActor
{
	GENERATED_BODY()

public:
	AXRWeapon();

	virtual void BeginPlay() override;

	/** 이 무기의 기준값. 에셋이 지정되지 않았으면 클래스 기본값을 쓴다. */
	const UXRWeaponData* GetWeaponData() const;

	/** 총구. 위치 = 탄이 나가는 점, X축 = 탄이 나가는 방향 */
	USceneComponent* GetMuzzle() const { return Muzzle; }
	FTransform GetMuzzleTransform() const;

protected:
	/** 무기별 기준값 에셋. 비워 두면 UXRWeaponData의 기본값 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UXRWeaponData> WeaponData;

	/** 몸체 색 (임시 모양용) */
	UPROPERTY(EditAnywhere, Category = "Weapon|Visual")
	FLinearColor BodyColor = FLinearColor(0.05f, 0.05f, 0.06f);

	/** 원점 = 손잡이를 쥔 손의 중심 */
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<USceneComponent> GripPoint;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<USceneComponent> Muzzle;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Visual")
	TObjectPtr<UStaticMeshComponent> HandleMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Visual")
	TObjectPtr<UStaticMeshComponent> ReceiverMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Visual")
	TObjectPtr<UStaticMeshComponent> BarrelMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Visual")
	TObjectPtr<UStaticMeshComponent> StockMesh;
};
