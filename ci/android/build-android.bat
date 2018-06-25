@REM Pull in nmake
set VSCMD_START_DIR=%CD%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1

set SOURCE_ROOT=%CD%

REM ARM32 bit
set TARGET_ARCHITECTURE=armeabi-v7a
if "%BuildPlatformAndroid%" == "arm32" set TARGET_ARCHITECTURE=armeabi-v7a

REM ARM64 bit (TODO: make this a build parameter)
if "%BuildPlatformAndroid%" == "arm64" set TARGET_ARCHITECTURE=arm64-v8a


mkdir build && ^
cd build && ^
cmake -G "NMake Makefiles" ^
    -Duse_default_uuid=ON ^
    -DCMAKE_SYSTEM_NAME=Android ^
    -DCMAKE_SYSTEM_VERSION=26 ^
    -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang ^
    -DCMAKE_ANDROID_ARCH_ABI=%TARGET_ARCHITECTURE% ^
    -DCMAKE_ANDROID_STL_TYPE=c++_static ^
    -DOPENSSL_ROOT_DIR="%SOURCE_ROOT%\external\android_ssl_curl\libs\%TARGET_ARCHITECTURE%" ^
    -DCURL_LIBRARY="%SOURCE_ROOT%\external\android_ssl_curl\libs\%TARGET_ARCHITECTURE%\lib\libcurl.a" ^
    -DCURL_INCLUDE_DIR="%SOURCE_ROOT%\external\android_ssl_curl\libs\%TARGET_ARCHITECTURE%\include" ^
    -DCMAKE_BUILD_TYPE="%BuildConfiguration%" ^
    %SOURCE_ROOT% ^
  && ^
cmake --build .
