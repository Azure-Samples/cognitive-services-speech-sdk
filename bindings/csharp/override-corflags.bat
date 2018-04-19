setlocal

@REM N.B. don't clash with any environment variable name that vcvars64.bat might set.
set PARAM_PLATFORM=%~1
set PARAM_CONFIGURATION=%~2
set PARAM_ASSEMBLY=%~3

@REM If corflags cannot be found, activate the VS 2017 dev prompt.
where corflags
if errorlevel 1 (
  set VSCMD_START_DIR=%CD%
  call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1
)

if "%PARAM_PLATFORM%" == "Win32" (
  corflags "%PARAM_ASSEMBLY%" /32bitreq-
  @REM Resulting assembly should be  AnyCPU (ILONLY, PE32, 32BITREQ-,
  @REM 32BITPREF-). We report the flags here for reference in the build log:
  corflags "%PARAM_ASSEMBLY%"
) else (
  echo Platform is not Win32, nothing to do.
)
