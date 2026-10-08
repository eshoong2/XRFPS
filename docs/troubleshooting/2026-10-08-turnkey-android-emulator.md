# Turnkey Android 설치 중 "Do you want to attempt again?"

- 날짜: 2026-10-08 / M0 단계 4
- 환경: UE 5.8.3 (Launcher), Windows 11, 기존 Android 설치 없음

## 증상

에디터 `Platforms → SDK Management → Android → Install Sdk` 실행 중 "턴키" 창에 **"Do you want to attempt again?"** 이 뜸.

## 원인 조사

에디터 로그(`<프로젝트>\Saved\Logs\<프로젝트>.log`)에서 `UATHelper` 줄을 확인했다.

| 단계 | 결과 |
|---|---|
| Android Studio 2024.1.2 설치 (`/S`) | 첫 시도 Exit code 1223 (사용자 취소 = UAC 창 처리 지연으로 추정), 이후 설치됨 |
| cmdline-tools | 성공 |
| `SetupAndroid.bat android-36 36.0.0 3.22.1 27.2.12479018` | 성공 (Finished with 0) |
| `SetupAndroidEmulator.bat pixel_6 36 48G 6144` | **실패 (Exit code 7)** |

실패한 것은 **Android 에뮬레이터(가상 Pixel 6, 디스크 48GB·RAM 6GB) 생성**뿐이었다. 이 프로젝트는 Quest 3S 실기기로만 테스트하고, 휴대폰 에뮬레이터로는 VR 앱을 실행할 수 없으므로 필요 없는 단계다.

같은 시각 로그의 `Ensure condition failed: IsInGameThread()`(AppTime.cpp)는 렌더러 쪽 handled ensure로, Android 설치와 무관하다.

## 해결

1. 재시도 창에서 **아니오** 선택 → 에디터 종료 (새 환경변수를 반영하기 위해)
2. Epic 검증 명령으로 상태 확인:

   ```
   RunUAT.bat Turnkey -command=VerifySdk -platform=Android -unattended
   ```

   결과: `Android: (Status=Valid, MinAllowed_Sdk=r27c, MaxAllowed_Sdk=r29, Current_Sdk=r27c, ...)`

## 설치된 결과

| 항목 | 값 |
|---|---|
| NDK | 27.2.12479018 (r27c) |
| SDK Platform / Build-tools | android-36 / 36.0.0 |
| CMake | 3.22.1 |
| Java | Android Studio 내장 JBR **17.0.11** |
| 환경변수 (사용자) | `ANDROID_HOME`, `NDKROOT`, `NDK_ROOT`, `JAVA_HOME` |
| adb | SDK platform-tools 37.0.1 |

## 배운 점

- Turnkey 재시도 창이 떠도 바로 누르지 말고 **로그에서 어떤 하위 단계가 실패했는지** 먼저 본다. 이번엔 핵심 SDK는 성공했고 선택적 단계만 실패했다.
- 사전 조사에서는 Epic 릴리스 노트를 근거로 "OpenJDK 21"을 예상했지만, Epic의 자동 설치가 실제로 설정한 Java는 Android Studio 내장 **JBR 17**이었다. **문서보다 공식 도구의 실제 결과와 검증 명령(VerifySdk)을 우선**한다.
- 빌드용 SDK Platform은 36이 설치됐다. Quest용 **targetSdk 34**는 프로젝트 설정에서 따로 지정한다 (ADR-0003).
