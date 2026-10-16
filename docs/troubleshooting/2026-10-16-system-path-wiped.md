# 시스템 PATH가 비어 git을 찾지 못함

- 날짜: 2026-10-16 (발견) / 원인 발생 추정: 2026-10-08
- 영향 범위: 새로 여는 모든 터미널·프로그램

## 증상

새 명령 프롬프트에서 `git switch main` 실행 시:

```
'git'은(는) 내부 또는 외부 명령, 실행할 수 있는 프로그램, 또는 배치 파일이 아닙니다.
```

git은 `C:\Program Files\Git`에 설치되어 있었고, 이전에는 정상 동작했다.

## 원인 조사

1. `git.exe` 위치 확인 → 설치되어 있음
2. 레지스트리의 시스템 PATH 확인 → **`C:\Program Files\GitHub CLI\` 한 줄만 남아 있음**
   - `C:\WINDOWS\system32` 등 Windows 기본 경로까지 모두 사라짐
   - 값 형식도 기본값(REG_EXPAND_SZ)이 아닌 REG_SZ로 바뀌어 있음
3. 경위 추정
   - 2026-10-08 M0 단계 2에서 시스템 PATH의 `%JAVA_HOME%\bin` 항목을 지우는 과정에서 다른 항목까지 함께 삭제된 것으로 보임
   - 이후 GitHub CLI 설치 프로그램이 자기 경로 한 줄만 추가

## 놓친 신호

2026-10-10 Claude가 Turnkey 검증 명령을 실행할 때, 레지스트리 PATH로 환경을 다시 읽자 `cmd`를 찾지 못하는 오류가 났다.
당시에는 "명령어 작성 문제"로 판단하고 `cmd.exe` 전체 경로로 우회했는데, 실제로는 이 문제의 첫 신호였다.

→ **이상한 오류를 우회로만 넘기지 말고 원인을 확인한다.**

## 해결

원래 값의 백업이 없어서, PC에 실제로 존재하는 폴더를 확인해 시스템 PATH를 재구성했다.
(시스템 설정이므로 사용자가 직접 "시스템 환경 변수 편집"에서 입력)

```
C:\WINDOWS\system32
C:\WINDOWS
C:\WINDOWS\System32\Wbem
C:\WINDOWS\System32\WindowsPowerShell\v1.0\
C:\WINDOWS\System32\OpenSSH\
C:\Program Files\Git\cmd
C:\Program Files\Git LFS
C:\Program Files\dotnet\
C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\
C:\Program Files\Microsoft SQL Server\150\Tools\Binn\
C:\Program Files\Microsoft SQL Server\Client SDK\ODBC\170\Tools\Binn\
C:\Program Files\GitHub CLI\
```

- `%SystemRoot%` 대신 절대 경로를 사용: 값 형식이 REG_SZ라 변수가 확장되지 않을 수 있기 때문
- 재구성한 목록이라 원래와 완전히 같지 않을 수 있음 → 이후 "명령을 찾을 수 없음"이 나오면 해당 경로만 추가

## 검증

새 PATH로 띄운 `cmd`에서 확인:

```
where git        → C:\Program Files\Git\cmd\git.exe
where git-lfs    → C:\Program Files\Git LFS\git-lfs.exe
where gh         → C:\Program Files\GitHub CLI\gh.exe
where powershell → C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe
git --version    → git version 2.50.0.windows.1
```

## 배운 점

- 시스템 환경 변수를 고치기 전에 **기존 값을 텍스트로 백업**한다.
- PATH 변경은 **새로 연 프로그램에만** 적용된다. 확인도 새 프로세스에서 한다.
- 편집 후에는 "지우려던 것만 지워졌는지" 전체 목록을 다시 확인한다.
