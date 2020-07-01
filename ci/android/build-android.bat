@REM Pull in nmake
set VSCMD_START_DIR=%CD%
call "c:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1

set SOURCE_ROOT=%~dp0\..\..

pushd "%SOURCE_ROOT%" && ^
mkdir build && ^
cd build && ^
cmake -G "NMake Makefiles" ^
    -DSPEECHSDK_BUILD_TYPE=%SPEECHSDK_BUILD_TYPE% ^
    -DSPEECHSDK_VERSION=%SPEECHSDK_SEMVER2NOMETA% ^
    -DSPEECHSDK_TARGET_PLATFORM=%SPEECHSDK_TARGET_PLATFORM% ^
    -DEXTENSION_KWS_ENABLED=%SPEECHSDK_ENABLE_KWS% ^
    -DEXTENSION_AUDIOCOMPRESSION_ENABLED=%EXTENSION_AUDIOCOMPRESSION_ENABLED% ^
    -DCMAKE_BUILD_TYPE="%BuildConfiguration%" ^
    -DGST_ROOT="%GST_BUILD_CONFIGURATION%" ^
    -DEXTENSION_CODEC_ENABLED="%SPEECHSDK_ENABLE_CODEC%" ^
    .. ^
  && ^
cmake --build .
