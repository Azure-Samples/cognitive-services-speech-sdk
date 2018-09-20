@REM Pull in nmake
set VSCMD_START_DIR=%CD%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1

set SOURCE_ROOT=%~dp0\..\..

REM Determine target architecture (32-bit is default)
set TARGET_ARCHITECTURE=armeabi-v7a
if "%BuildPlatform%" == "arm64" set TARGET_ARCHITECTURE=arm64-v8a

pushd "%SOURCE_ROOT%" && ^
mkdir build && ^
cd build && ^
cmake -G "NMake Makefiles" ^
    -Duse_default_uuid=ON ^
    -DCMAKE_SYSTEM_NAME=Android ^
    -DCMAKE_SYSTEM_VERSION=21 ^
    -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang ^
    -DCMAKE_ANDROID_ARCH_ABI=%TARGET_ARCHITECTURE% ^
    -DCMAKE_ANDROID_STL_TYPE=c++_static ^
    -DOPENSSL_ROOT_DIR="%SOURCE_ROOT%\external\android_ssl_curl\libs\%TARGET_ARCHITECTURE%" ^
    -DCMAKE_BUILD_TYPE="%BuildConfiguration%" ^
    .. ^
  && ^
cmake --build .
