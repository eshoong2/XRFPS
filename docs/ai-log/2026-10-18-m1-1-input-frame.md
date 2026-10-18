# M1-1 입력 프레임과 데스크톱 입력 소스

- 날짜 / 마일스톤: 2026-10-18 / M1 (구현 순서 1단계)
- 소요 시간: (직접 기입)

## 사용한 도구와 모델
| 작업 | 도구 | 모델 | 선택 이유 |
|---|---|---|---|
| C++ 구현, 빌드, 헤드리스 실행 확인 | Claude Code | Claude Opus 5.5 | 엔진 소스에서 API를 확인하며 작성해야 하는 작업 |

## 코드 분담
- 처음 계획: 팀장(AI)이 구조와 예시(마우스 머리 회전)를 쓰고, 신입(나)이 손 조작 기능을 이어서 작성
- 변경: 진행 속도를 고려해 **"팀장 작성 → 신입 리뷰·설명"** 방식으로 전환 (M1 설계 때 미리 정해 둔 대안)
- 그래서 이 단계의 코드는 전부 AI가 작성했고, 나는 PR 리뷰에서 동작을 설명하고 에디터에서 직접 확인한다

## AI가 한 일
- `FXRInputFrame`, `UXRInputSource`, `UXRDesktopInputSource`, `UXRPoseComponent`, 최소 Pawn·GameMode 작성
- 엔진 소스로 API 확인
  - `UHeadMountedDisplayFunctionLibrary`가 5.8에서 XRBase 플러그인으로 옮겨진 것을 발견 → 엔진 기본 모듈의 `IXRTrackingSystem`을 직접 사용
  - Enhanced Input의 `AccumulationBehavior`, `EInputAxisSwizzle` 이름 확인
- 빌드 실패 1회 해결: 하위 폴더 include를 못 찾음 → Build.cs에 모듈 폴더 include 경로 추가 (Lyra 방식)
- 구현 중 설계 변경: 트리거·스틱을 "주 손 값"이 아니라 왼손·오른손 원본 값으로 저장 → 설계 문서 갱신

## AI가 틀렸거나 고친 부분
- 첫 빌드에서 include 경로 설정을 빠뜨림 (위 참고)
- PowerShell에서 `git commit -m`에 따옴표와 `->`가 든 메시지를 넘겨 커밋이 실패 → 메시지 파일(`-F`)로 해결

## 검증 방법
- 에디터 빌드 성공 (경고 0)
- 헤드리스 실행 로그에서 게임 모드 `XRFPSGameMode`, 입력 소스가 None에서 Desktop으로 선택됨 확인
- (리뷰어 확인 예정) 에디터 Play에서 키 조작으로 손 축 이동, 트리거 상태 표시

## 회고
-
