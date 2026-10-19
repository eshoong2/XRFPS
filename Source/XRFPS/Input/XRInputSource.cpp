// Copyright XRFPS. All Rights Reserved.

#include "Input/XRInputSource.h"

UXRInputSource::UXRInputSource()
{
	// 입력 소스는 스스로 Tick하지 않는다. UXRPoseComponent가 필요할 때 BuildFrame을 호출한다.
	PrimaryComponentTick.bCanEverTick = false;
}
