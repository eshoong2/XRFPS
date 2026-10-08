# 01. 개발 환경 구축 (M0)

- 작성일: 2026-10-07
- 근거: [ADR-0003](../decisions/ADR-0003-engine-toolchain-xr.md)
- M0 완료 기준: **빈 레벨이 Quest 3S에서 실행되고, Win64 빌드와 같은 Wi-Fi로 접속된다**

## 확정 구성

| 항목 | 구성 |
|---|---|
| 엔진 | UE 5.8.3 (Epic Launcher) |
| IDE / 컴파일러 | VS Community 2022 17.14 + MSVC 14.44.35207 (기존 설치) |
| Launcher 설치 옵션 | Core + Templates & Feature Packs + Target Platform: Android + Engine Source (Editor symbols 제외) |
| Android | Turnkey 자동 설치 (NDK r27c, SDK 36, Android Studio JBR 17) |
| XR | 엔진 기본 OpenXR만 |
| Quest 프로젝트 설정 | minSdk 32, targetSdk 34, arm64, Vulkan, Forward, MSAA 4x, Multi-View, Mobile HDR off |

## 설치 전 PC 상태 (2026-10-07 확인)

- VS 2022 17.14, MSVC 14.44.35207, Windows SDK 10.0.22621 / 10.0.26100 설치됨
- Oracle JDK 21.0.6 설치, 시스템 `JAVA_HOME` 설정됨 (사용하지 않음)
- Android SDK / Android Studio 없음, Epic Launcher 없음
- RAM 16GB 싱글 채널 → **2026-10-08 32GB 듀얼 채널로 증설 완료** (Channel A/B 각 16GB, 4800MT/s, 내장 GPU 예약 4GB → OS 가용 약 27GB)

## 체크리스트

### 단계 1. 계정과 기기 (시간이 걸리므로 먼저)
- [x] Meta 개발자 조직(팀) 생성
- [ ] 계정 인증 확인 → 단계 5에서 개발자 모드 토글이 켜지면 완료
- [x] Epic Games Launcher 설치, 로그인

### 단계 2. PC 정리
- [x] Oracle JDK 21 제거 (설정 > 앱)
- [x] 시스템 환경 변수 `JAVA_HOME` 삭제
- [x] 시스템 `Path`의 `%JAVA_HOME%\bin` 항목 삭제
- [x] VS Installer에서 **.NET 데스크톱 개발** 워크로드 추가

> 왜: Turnkey 자동 설치는 "기존 Java·Android 환경변수가 없는 깨끗한 PC"를 전제로 한다.

### 단계 3. 엔진 설치
- [x] Launcher > Unreal Engine > 라이브러리 > `+` > **5.8.0** 선택 (Launcher는 마이너 버전만 표시하고 최신 핫픽스를 설치함) → 설치 후 `Engine\Build\Build.version`으로 5.8.3 확인
- [x] 옵션 (설치 버튼 → 경로 창의 "옵션"): Core, Templates & Feature Packs, Engine Source, Target Platforms는 **Android만** / Editor symbols, MetaHuman Creator 코어 데이터 해제
- [x] 설치 화면에 표시된 용량: 다운로드 **13.7GB**, 필요 저장 공간 **39.0GB**
- [x] 설치 확인 (2026-10-08): `Build.version` = **5.8.3** (CL 58210709), 실제 39GB, Android 바이너리·엔진 소스·OpenXR 플러그인 포함
  - VR 템플릿은 Blueprint 버전(`TP_VirtualRealityBP`)만 제공 → C++ 프로젝트는 별도 생성하고 VR 템플릿은 구조 참고용

### 단계 4. Android 환경 (Turnkey)
- [x] 에디터 실행 → Platforms > SDK Management > Android > Install Sdk, 라이선스는 `Y`
  - 에뮬레이터 생성 단계만 실패 → "다시 시도" 창에서 **아니오** ([트러블슈팅](../troubleshooting/2026-10-08-turnkey-android-emulator.md))
- [x] 확인 (2026-10-08): `Turnkey -command=VerifySdk -platform=Android` → **Status=Valid**, NDK r27c, SDK 36 / build-tools 36.0.0, Java는 Android Studio JBR 17.0.11 (Turnkey가 설정한 값), `adb`는 SDK platform-tools

### 단계 5. Quest 연결
- [ ] Quest 개발자 모드 켜기 (Meta Horizon 앱)
- [ ] Oculus ADB Drivers 2.0 설치
- [ ] USB 연결 → 헤드셋에서 USB 디버깅 "항상 허용"
- [ ] `adb devices`에 기기 표시

### 단계 6. 프로젝트 생성
- [ ] C++ 프로젝트 생성, 경로 `C:\Dev\XRFPS` (기존 저장소 루트)
- [ ] 빌드 로그에서 MSVC 14.44 선택 확인
- [ ] OpenXR 켜기, 다른 벤더 XR 플러그인(Oculus, SteamVR, PICO) 끄기
- [ ] Android 설정: Package for Meta Quest devices, minSdk 32, targetSdk 34, arm64, Vulkan만
- [ ] 렌더링: Mobile Shading Path = Forward, MSAA 4x, Mobile Multi-View on, Mobile HDR off, Lumen/Nanite/VSM off

### 단계 7. Quest 첫 실행
- [ ] 빈 레벨을 Android(ASTC)로 패키징해 설치
- [ ] 생성된 `AndroidManifest.xml` 확인: `com.oculus.intent.category.VR`, `android.hardware.vr.headtracking`, supportedDevices에 `quest3s`, targetSdk 34, INTERNET 권한
- [ ] 실기기 확인: HMD 트래킹, 양손 Grip/Aim 포즈, 트리거·그립·스틱·A/B/X/Y

### 단계 8. 크로스플랫폼 접속
- [ ] Win64 Development 빌드를 `?listen`으로 실행
- [ ] Windows 방화벽에서 게임 exe 인바운드 허용 (UDP 7777)
- [ ] Quest에서 `open <PC IP>`로 접속 성공
- [ ] 양쪽은 같은 엔진 버전·같은 프로젝트 버전으로 빌드 (에디터 PIE를 서버로 쓰지 않음)

## 기록

| 단계 | 소요 시간 | 막힌 점 → troubleshooting 링크 |
|---|---|---|
| 1 | | |
| 2 | | |
| 3 | | |
| 4 | | |
| 5 | | |
| 6 | | |
| 7 | | |
| 8 | | |
