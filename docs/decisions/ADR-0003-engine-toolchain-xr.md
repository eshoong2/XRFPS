# ADR-0003: 엔진·툴체인·XR 플러그인 — UE 5.8.3 + VS 2022(MSVC 14.44) + 엔진 기본 OpenXR

- 날짜: 2026-10-07
- 상태: 채택
- 조사일: 2026-10-06 (버전·호환성은 이 날짜 기준)

## 배경

- 타깃: Meta Quest 3S **단독(Android) 빌드**, 개발 PC는 내장 GPU 노트북 ([ADR-0001](ADR-0001-dev-workflow.md))
- Quest 빌드는 엔진 버전마다 맞는 Android SDK/NDK/JDK와 C++ 컴파일러가 정해져 있고, 틀리면 빌드 단계에서 크게 막힌다.
- 공식 문서끼리도 값이 다른 경우가 많아(아래 "문서 간 모순"), 1차 출처를 교차 검증해 결정했다.

## 결정 1: 엔진 버전 — UE 5.8.3

| | **5.8.3** ✅ | 5.7.4 |
|---|---|---|
| Quest 3S | 엔진에 Quest 3S 디바이스 프로필 공식 추가 | 없음 |
| 유지보수 | 핫픽스 진행 중 (5.8.3: 2026-09-22) | 2026-03 이후 수정 없음 |
| Android 설정 | Turnkey가 cmdline-tools·SDK·NDK·JDK·환경변수까지 자동 설치 | cmdline-tools 수동 선설치 필요 |
| 알려진 문제 | 아래 함정 3개 (설정으로 회피 가능) | Deferred 모바일 렌더러 크래시 등 보고 |

**5.8.3에서 반드시 피해야 할 함정**
1. MSVC 14.51에서 5.8 빌드 실패 → 14.44 또는 14.50 사용 (Epic 직원 답변)
2. 5.8.2부터 Target SDK 기본값 36 → Meta는 몰입형 앱에 **34** 요구 → 직접 지정
3. 5.8부터 모바일 기본 렌더러가 Deferred → **Forward + MSAA 4x + Multi-View** 직접 지정

## 결정 2: C++ 툴체인 — 기존 VS 2022 17.14 + MSVC 14.44.35207

| | **VS 2022 + 14.44** ✅ | VS 2026 + 14.50 |
|---|---|---|
| 설치 상태 | 이미 설치됨 | 새로 설치 (수십 GB) |
| 호환성 | Epic Launcher 5.8 빌드팜과 **동일 컴파일러** | Epic 권장, 14.50 수동 선택 필요 |
| 위험 | 향후 UE 버전에서 VS 2022 지원 축소 가능 (5.8이 마지막 UE5라 영향 작음) | 기본 14.51이 깔리면 빌드 실패 |

처음에는 VS 2026을 추천했으나, PC를 확인해 보니 VS 2022 17.14 + MSVC 14.44가 이미 설치돼 있어 변경했다.

## 결정 3: XR 플러그인 — 엔진 기본 OpenXR만 사용

| | **엔진 기본 OpenXR** ✅ | Meta XR 플러그인 v207 |
|---|---|---|
| MVP (HMD + 컨트롤러) | 가능. Meta 공식: Epic OpenXR만으로 Quest 앱 빌드·출시 가능 | 가능 |
| Meta XR Simulator | 해당 없음 | **AMD GPU 미지원**, **Listen Server에서 크래시** (공식 known issue) |
| 엔진 버전 결합 | 엔진 내장 | 엔진 버전 하나에 묶임, "5.8 로드 실패" 피드백 조사 중 |
| Meta 전용 기능 | 일부 없음 | 있음 |

플러그인의 핵심 이점(Simulator)을 이 PC에서 쓸 수 없으므로 위험만 늘어난다.

**추상화 경계**: 게임 코드는 `UMotionControllerComponent`, OpenXR Grip/Aim 포즈, Enhanced Input 같은 엔진 표준 API만 사용한다. 손 트래킹 단계에서 Meta 플러그인을 별도 ADR로 재검토할 때 재작성 범위를 줄이기 위해서다.

Meta 포크 엔진(소스 빌드)은 디스크 약 400GB와 대용량 RAM이 필요해 검토 대상에서 제외했다.

## 문서 간 모순과 해소

| 항목 | 충돌 | 결론 |
|---|---|---|
| Target SDK | Epic 문서 35 / 5.8 릴리스 노트 34 / 5.8.2 핫픽스 36 / Meta 34 요구 | **34로 직접 지정** |
| NDK | Meta 문서 25.1 / Epic 5.8 r27c | **r27c** (Meta 문서는 플러그인 사용 시 기준이고 2026-04 이후 미갱신) |
| JDK | 구 문서 17 / 5.8 릴리스 노트 OpenJDK 21.0.3 | **21** (Turnkey 설치본) |
| VS 2026 | 문서상 18.0+ 지원 / 18.6부터 기본 MSVC 14.51은 빌드 실패 | 14.51 사용 금지 |
| 모바일 렌더러 | Meta 문서 "Quest 기본 Forward" / 5.8 기본 Deferred | **Forward 직접 지정** |

## 이 결정이 만든 제약

- **에디터(PIE)를 서버로 쓸 수 없다.** 쿡되지 않은 에디터와 쿡된 Android 빌드 사이에 NetChecksumMismatch가 보고됨 → 크로스플랫폼 테스트 서버는 **패키징한 Win64 빌드**. (M2 설계에 반영)
- HMD·모션 컨트롤러 Transform은 자동 복제되지 않는다 → 클라이언트→서버 포즈 전송 경로를 M2에서 설계.

## 출처

- UE 5.8 릴리스 노트: https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-5-8-release-notes
- UE 5.8.3 핫픽스: https://forums.unrealengine.com/t/5-8-3-hotfix-released/2833315
- UE 5.8.2 핫픽스 (Target SDK 36): https://forums.unrealengine.com/t/5-8-2-hotfix-released/2746335
- Android 요구사항: https://dev.epicgames.com/documentation/en-us/unreal-engine/android-development-requirements-for-unreal-engine
- Turnkey 자동 설정: https://dev.epicgames.com/documentation/unreal-engine/automated-android-sdk-ndk-and-jdk-setup-in-unreal-engine
- VS 설정 (5.8): https://dev.epicgames.com/documentation/en-us/unreal-engine/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine?application_version=5.8
- MSVC 14.51 문제: https://forums.unrealengine.com/t/msvc-removes-hash-map-in-14-51/2743134
- Meta 매니페스트 요구사항: https://developers.meta.com/horizon/resources/publish-mobile-manifest/
- Meta 설치 방식 비교: https://developers.meta.com/horizon/documentation/unreal/unreal-quick-start-install-unreal-engine/
- Meta 호환성 매트릭스: https://developers.meta.com/horizon/documentation/unreal/unreal-compatibility-matrix/
- XR Simulator AMD 미지원: https://developers.meta.com/horizon/feedback/vr/investigations/835206772163776/
- XR Simulator 시작하기 (Listen Server 크래시): https://developers.meta.com/horizon/documentation/unreal/xrsim-getting-started/
