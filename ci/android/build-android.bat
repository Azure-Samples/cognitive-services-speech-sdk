@REM Pull in nmake
set VSCMD_START_DIR=%CD%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1

set SOURCE_ROOT=%~dp0\..\..

pushd "%SOURCE_ROOT%" && ^
mkdir build && ^
cd build && ^
cmake -G "NMake Makefiles" ^
    -DSPEECHSDK_BUILD_TYPE=%SPEECHSDK_BUILD_TYPE% ^
    -DSPEECHSDK_VERSION=%SPEECHSDK_SEMVER2NOMETA% ^
    -DSPEECHSDK_TARGET_PLATFORM=%SPEECHSDK_TARGET_PLATFORM% ^
    -DCMAKE_BUILD_TYPE="%BuildConfiguration%" ^
    .. ^
  && ^
cmake --build .
