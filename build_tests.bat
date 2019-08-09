mkdir .\tests\bin
mkdir .\tests\obj

setlocal enabledelayedexpansion

set "build_ok=1"
set INCLUDE=%INCLUDE%

for /f %%f in ('dir /b ".\tests\*.cpp"') do (
  echo "compiling test %VisualStudioVersion% %%~nf"
  cl -EHsc -W4 -Fo.\tests\obj\%%~nf.obj -D _CRT_SECURE_NO_WARNINGS -c ".\tests\%%f"
  IF ERRORLEVEL 1 (
    set "build_ok=0"
  )

  if "%build_ok%"=="1" (
    cl .\tests\obj\%%~nf.obj -D _CRT_SECURE_NO_WARNINGS -Fe.\tests\bin\%%~nf.exe
    IF ERRORLEVEL 1 (
      set "build_ok=0"
    )
  )
)

if /I "%build_ok%" NEQ "1" (
  echo "tests build failed"
  exit /B 1
)

echo "tests build ok"

del /Q .\tests\obj\*.obj