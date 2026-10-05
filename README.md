# XRFPS

Meta Quest 3S용 **서버 권위형 XR 멀티플레이 FPS**.
HMD와 손/컨트롤러 트래킹을 활용해 플레이어의 **실제 자세와 조준 상태가 전투 성능에 영향**을 준다.

> 🚧 진행 중 — 현재 단계: **M0 (환경 구축)**

<!-- 대표 GIF: docs/images/demo.gif 를 추가한 뒤 아래 주석을 해제
![demo](docs/images/demo.gif)
-->

## 핵심 특징 (MVP)

- **무기 핸들링**: 서버가 원본 트래킹 데이터로 조준 안정도를 계산해 탄 퍼짐에 반영
- **양손 파지**: 보조 손으로 총열을 잡으면 흔들림과 반동이 줄어듦
- **서버 권위**: 클라이언트는 트래킹 원본만 보내고, 서버가 검증·판정

## 기술 스택

- Unreal Engine 5 (C++ 중심, Blueprint는 UI·연출·데이터 세팅)
- Meta Quest 3S (단독 Android 빌드)

## 로드맵

| 단계 | 내용 | 상태 |
|---|---|---|
| M0 | 환경 구축 | 진행 중 |
| M1 | 싱글 플레이: VR 손, 총 잡기, 사격, 양손 파지 | |
| M2 | 네트워크: 트래킹 동기화, 서버 검증, 서버 판정 | |
| M3 | 조준 안정도 + 탄 퍼짐 → MVP | |
| M4~ | 실제 자세, 지연 보정 | |

## AI 활용 방식

이 프로젝트는 **AI 도구와 모델을 직접 선택하고 검증하며 개발하는 과정**을 함께 기록한다.

- 진행 순서: 목표 정의 → 설계 논의 → 대안 비교 → **결정(개발자)** → 구현 → 리뷰 → 기록
- 설계 합의 전에는 AI가 대규모 코드를 작성하지 않는다. 최종 결정은 개발자가 한다.
- 작업마다 사용한 모델, 선택 이유, AI의 오류와 수정, 검증 방법을 [`docs/ai-log/`](docs/ai-log/)에 남긴다.

## 저장소 구조

```
XRFPS/
├─ README.md                 ← 이 문서 (프로젝트 소개)
├─ docs/
│  ├─ design/                ← 기능별 설계 문서
│  ├─ decisions/             ← 결정 기록(ADR)
│  ├─ troubleshooting/       ← 버그 증상·원인·해결
│  ├─ ai-log/                ← AI 활용 기록
│  ├─ portfolio/             ← 노션 포트폴리오 페이지 원본
│  └─ images/                ← README·노션용 스크린샷, GIF
│
│  (M0 이후 UE 프로젝트가 루트에 추가됨)
├─ XRFPS.uproject
├─ Config/
├─ Source/XRFPS/             ← C++ 게임 로직
└─ Content/                  ← 에셋 (Git LFS)
```

- `.uasset`, `.umap` 등 바이너리 에셋은 **Git LFS**로 관리한다. ([.gitattributes](.gitattributes))

## 문서

| 폴더 | 내용 |
|---|---|
| [docs/design/](docs/design/) | 기능별 설계 문서 — 시작은 [00-vision.md](docs/design/00-vision.md) |
| [docs/decisions/](docs/decisions/) | 결정 기록(ADR) |
| [docs/troubleshooting/](docs/troubleshooting/) | 버그의 증상·원인·해결 |
| [docs/ai-log/](docs/ai-log/) | AI 활용 기록 |
