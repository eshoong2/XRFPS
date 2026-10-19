// Copyright XRFPS. All Rights Reserved.

#include "Weapon/XRWeapon.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Weapon/XRWeaponData.h"

namespace XRWeaponShape
{
	// 소총 임시 모양 치수(cm). 원점 = 손잡이를 쥔 손의 중심
	constexpr float BarrelHeight = 9.f;		// 총열 중심선의 높이 (손보다 위)
	constexpr float ReceiverFront = 25.5f;	// 몸체 앞 끝
	constexpr float BarrelLength = 40.f;
	constexpr float MuzzleX = ReceiverFront + BarrelLength;	// 65.5

	// 엔진 기본 도형은 Cube 100cm, Cylinder 높이 100cm·지름 100cm라서 cm를 100으로 나눈 값이 스케일이다
	constexpr float CmToScale = 0.01f;
}

AXRWeapon::AXRWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ShapeMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

	GripPoint = CreateDefaultSubobject<USceneComponent>(TEXT("GripPoint"));
	RootComponent = GripPoint;

	using namespace XRWeaponShape;

	// 메시 하나 만들기: 위치·크기(cm)·회전을 받아 붙인다. 총에 맞아 판정이 막히지 않도록 충돌은 끈다.
	// ShapeMaterial은 static 지역 변수라 캡처하지 않아도 람다 안에서 바로 쓸 수 있다
	auto CreatePart = [this](const TCHAR* Name, UStaticMesh* Mesh, const FVector& CenterCm, const FVector& SizeCm, const FRotator& Rotation)
	{
		UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		Part->SetupAttachment(GripPoint);
		Part->SetStaticMesh(Mesh);
		Part->SetRelativeLocation(CenterCm);
		Part->SetRelativeRotation(Rotation);
		Part->SetRelativeScale3D(SizeCm * CmToScale);
		Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Part->SetGenerateOverlapEvents(false);
		Part->CastShadow = false;
		if (ShapeMaterial.Succeeded())
		{
			Part->SetMaterial(0, ShapeMaterial.Object);
		}
		return Part;
	};

	UStaticMesh* Cube = CubeMesh.Object;
	UStaticMesh* Cylinder = CylinderMesh.Object;

	// 손잡이: 손이 쥐는 부분. 원점(손 중심)에 세로로 선다
	HandleMesh = CreatePart(TEXT("HandleMesh"), Cube, FVector(0.f, 0.f, 0.f), FVector(3.f, 2.5f, 10.f), FRotator::ZeroRotator);

	// 몸체: 손 위쪽, 앞뒤로 35cm
	ReceiverMesh = CreatePart(TEXT("ReceiverMesh"), Cube, FVector(8.f, 0.f, 7.f), FVector(35.f, 4.f, 8.f), FRotator::ZeroRotator);

	// 총열: 원기둥은 기본이 Z축 방향이라 피치 -90도로 눕혀 X축(앞)을 향하게 한다
	BarrelMesh = CreatePart(TEXT("BarrelMesh"), Cylinder, FVector(ReceiverFront + BarrelLength * 0.5f, 0.f, BarrelHeight), FVector(2.4f, 2.4f, BarrelLength), FRotator(-90.f, 0.f, 0.f));

	// 개머리판: 몸체 뒤
	StockMesh = CreatePart(TEXT("StockMesh"), Cube, FVector(-19.5f, 0.f, 5.f), FVector(20.f, 3.5f, 8.f), FRotator::ZeroRotator);

	// 총구: 총열 앞 끝. X축이 탄이 나가는 방향
	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(GripPoint);
	Muzzle->SetRelativeLocation(FVector(MuzzleX, 0.f, BarrelHeight));
}

void AXRWeapon::BeginPlay()
{
	Super::BeginPlay();

	// 기본 도형 머티리얼의 Color 파라미터로 몸체 색을 칠한다 (임시 모양용)
	TArray<UStaticMeshComponent*> Parts;
	GetComponents(Parts);
	for (UStaticMeshComponent* Part : Parts)
	{
		if (UMaterialInstanceDynamic* Material = Part->CreateAndSetMaterialInstanceDynamic(0))
		{
			Material->SetVectorParameterValue(TEXT("Color"), BodyColor);
		}
	}
}

const UXRWeaponData* AXRWeapon::GetWeaponData() const
{
	return WeaponData ? WeaponData.Get() : GetDefault<UXRWeaponData>();
}

FTransform AXRWeapon::GetMuzzleTransform() const
{
	return Muzzle ? Muzzle->GetComponentTransform() : GetActorTransform();
}
