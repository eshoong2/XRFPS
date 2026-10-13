# C++ 프로젝트 뼈대와 Quest 설정 (M0 단계 6)

- 날짜 / 마일스톤: 2026-10-13 / M0
- 소요 시간: (직접 기입)

## 사용한 도구와 모델
| 작업 | 도구 | 모델 | 선택 이유 |
|---|---|---|---|
| 설정 키·값 확인, 파일 작성, 빌드 | Claude Code | Claude Opus 5.5 | 엔진 소스를 직접 읽어 근거를 확인해야 하는 작업 |

## AI가 한 일 / 내가 한 일
- AI:
  - 설정 키 이름과 enum 값을 **설치된 엔진 소스**에서 확인 (`AndroidRuntimeSettings.h`, `RendererSettings.h`, `EngineTypes.h`)
  - Epic VR 템플릿 설정과 비교하고, 다르게 한 값(`bPackageForOpenXRImmersive`, `r.ReflectionMethod`, `r.GenerateMeshDistanceFields`)은 이유를 ini 주석으로 남김
  - 엔진 코드로 사전 조사의 불확실 항목 해소: INTERNET 권한 기본 포함(`UEDeployAndroid.cs`), supportedDevices 자동 추가 안 됨(`OpenXR_APL.xml`)
  - 한국어 주석이 든 BOM 없는 ini를 엔진이 UTF-8로 읽는지 `FileHelper.cpp`로 확인
  - 명령줄 빌드로 MSVC 14.44 선택 확인
- 나:
  - 파일 검토 후 사용 결정, 진행 순서 결정 (단계 6 → M1 설계 → Quest 연결 → M0 완료 → M1 구현)

## AI가 틀렸거나 내가 고친 부분
- **요청을 넘어서 진행함**: "Quest 연결은 건너뛰고 진행하자"를 다음 단계 실행으로 받아들여, 브리핑 없이 파일을 만들고 빌드를 시도했다. 내가 원한 건 "건너뛰어도 되는지, 언제까지 해야 하는지, 다음은 무엇인지"에 대한 브리핑이었다.
  → 애매한 지시는 실행 전에 의도를 확인한다.
- **근거의 출처**: AI가 프로젝트 뼈대를 연습 프로젝트(AimTrainer)에서 가져왔다고 해서 "왜 가져왔냐"고 물었다. 확인해 보니 내용은 Epic 공식 템플릿(`TP_Blank`)과 같았지만, AI는 처음부터 공식 템플릿을 1차 근거로 삼았어야 했다.
  → 포트폴리오 코드의 근거는 공식 자료로 설명할 수 있어야 한다.

## 검증 방법
- 설정값: 엔진 헤더의 `ConsoleVariable`·enum 정의와 대조
- 빌드: `Build.bat XRFPSEditor Win64 Development` 성공, 로그의 툴체인 버전 확인
- (예정) 에디터 Project Settings 화면에서 값 확인, 단계 7에서 생성된 AndroidManifest.xml 확인

## 회고
-
