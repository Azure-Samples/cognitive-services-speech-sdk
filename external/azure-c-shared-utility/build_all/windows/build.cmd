@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

set CMAKE_DIR=shared-util_Win32
set MAKE_NUGET_PKG=no

set current-path=%~dp0
rem // remove trailing slash
set current-path=%current-path:~0,-1%

set build-root=%current-path%\..\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

rem -----------------------------------------------------------------------------
rem -- parse script arguments
rem -----------------------------------------------------------------------------

rem // default build options
set build-clean=0
set build-config=
set build-platform=Win32

:args-loop
if "%1" equ "" goto args-done
if "%1" equ "-c" goto arg-build-clean
if "%1" equ "--clean" goto arg-build-clean
if "%1" equ "--config" goto arg-build-config
if "%1" equ "--platform" goto arg-build-platform
if "%1" equ "--make_nuget" goto arg-build-nuget
call :usage && exit /b 1

:arg-build-clean
set build-clean=1
goto args-continue

:arg-build-config
shift
if "%1" equ "" call :usage && exit /b 1
set build-config=%1
goto args-continue

:arg-build-platform
shift
if "%1" equ "" call :usage && exit /b 1
set build-platform=%1
if %build-platform% == x64 (
	set CMAKE_DIR=shared-util_x64
) else if %build-platform% == arm (
	set CMAKE_DIR=shared-util_arm
)
goto args-continue

:arg-build-nuget
shift
if "%1" equ "" call :usage && exit /b 1
set MAKE_NUGET_PKG=%1
goto args-continue

:args-continue
shift
goto args-loop

:args-done

rem -----------------------------------------------------------------------------
rem -- build with CMAKE
rem -----------------------------------------------------------------------------

echo CMAKE Output Path: %build-root%\cmake\%CMAKE_DIR%

if NOT EXIST %build-root%\cmake\%CMAKE_DIR% GOTO NO_CMAKE_DIR
rmdir /s/q %build-root%\cmake\%CMAKE_DIR%
rem no error checking
:NO_CMAKE_DIR

mkdir %build-root%\cmake\%CMAKE_DIR%
rem no error checking

pushd %build-root%\cmake\%CMAKE_DIR%

if %MAKE_NUGET_PKG% == yes (
	echo ***Running CMAKE for Win32 ***
	cmake -Dskip_unittests:BOOL=ON %build-root%
	if not %errorlevel%==0 exit /b %errorlevel%
	popd

	echo *** Running CMAKE x64 ***
	if EXIST %build-root%\cmake\shared-util_x64 (
		rmdir /s/q %build-root%\cmake\shared-util_x64
	)

	mkdir %build-root%\cmake\shared-util_x64
	pushd %build-root%\cmake\shared-util_x64
	cmake -Dskip_unittests:BOOL=ON %build-root% -G "Visual Studio 14 Win64"
	if not %errorlevel%==0 exit /b %errorlevel%
	popd

	echo *** Running CMAKE ARM ***
	if EXIST %build-root%\cmake\shared-util_arm (
		rmdir /s/q %build-root%\cmake\shared-util_arm
	)
	mkdir %build-root%\cmake\shared-util_arm
	pushd %build-root%\cmake\shared-util_arm
	cmake -Dskip_unittests:BOOL=ON %build-root% -G "Visual Studio 14 ARM"
	if not %errorlevel%==0 exit /b %errorlevel%
) else if %build-platform% == Win32 (
	echo ***Running CMAKE for Win32***
	cmake %build-root%
	if not %errorlevel%==0 exit /b %errorlevel%
) else if %build-platform% == ARM (
	echo ***Running CMAKE for ARM***
	cmake %build-root% -G "Visual Studio 14 ARM"
	if not %errorlevel%==0 exit /b %errorlevel%
) else (
	echo ***Running CMAKE for Win64***
	cmake %build-root% -G "Visual Studio 14 Win64"
	if not %errorlevel%==0 exit /b %errorlevel%
)

if %MAKE_NUGET_PKG% == yes (
		echo running Win32 Shared Util
		echo ***Building all configurations***
		msbuild /m %build-root%\cmake\shared-util_Win32\azure_c_shared_utility.sln /p:Configuration=Release
		msbuild /m %build-root%\cmake\shared-util_Win32\azure_c_shared_utility.sln /p:Configuration=Debug
		if not %errorlevel%==0 exit /b %errorlevel%

		msbuild /m %build-root%\cmake\shared-util_x64\azure_c_shared_utility.sln /p:Configuration=Release
		msbuild /m %build-root%\cmake\shared-util_x64\azure_c_shared_utility.sln /p:Configuration=Debug
		if not %errorlevel%==0 exit /b %errorlevel%

		msbuild /m %build-root%\cmake\shared-util_arm\azure_c_shared_utility.sln /p:Configuration=Release
		msbuild /m %build-root%\cmake\shared-util_arm\azure_c_shared_utility.sln /p:Configuration=Debug
		if not %errorlevel%==0 exit /b %errorlevel%
) else (
	if not defined build-config (
		echo ***Building both configurations***
		msbuild /m azure_c_shared_utility.sln /p:Configuration=Release
		msbuild /m azure_c_shared_utility.sln /p:Configuration=Debug
		if not %errorlevel%==0 exit /b %errorlevel%
	) else (
		echo ***Building %build-config% only***
		msbuild /m azure_c_shared_utility.sln /p:Configuration=%build-config%
		if not %errorlevel%==0 exit /b %errorlevel%
	)

	ctest -C "debug" -V
	if not %errorlevel%==0 exit /b %errorlevel%
)

popd

goto :eof

:usage
echo build.cmd [options]
echo options:
echo  --clean                 [] Cleans the project
echo  --config ^<value^>      [Debug] build configuration (e.g. Debug, Release)
echo  --platform ^<value^>    [Win32] build platform (e.g. Win32, x64, arm, ...)
echo  --make_nuget ^<value^>  [no] generates the binaries to be used for nuget packaging (e.g. yes, no)
goto :eof
