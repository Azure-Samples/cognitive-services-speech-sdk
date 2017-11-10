@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableExtensions EnableDelayedExpansion
@echo off

rem -----------------------------------------------------------------------------
rem -- setup path information
rem -----------------------------------------------------------------------------

set current-path=%~dp0
rem // remove trailing slash
set current-path=%current-path:~0,-1%

echo Current Path: %current-path%

set build-root=%current-path%\..\..\..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

set client-root=%current-path%\..\..\..
for %%i in ("%client-root%") do set client-root=%%~fi

where /q nuget.exe
if not !errorlevel! == 0 (
@Echo Azure Shared Utility needs to download nuget.exe from https://www.nuget.org/nuget.exe 
@Echo https://www.nuget.org 
choice /C yn /M "Do you want to download and run nuget.exe?" 
if not !errorlevel!==1 goto :eof
rem if nuget.exe is not found, then ask user
Powershell.exe wget -outf nuget.exe https://nuget.org/nuget.exe
	if not exist .\nuget.exe (
		echo nuget does not exist
		exit /b 1
	)
)

set build-path=%build-root%\cmake

echo Build root is %build-root%
echo Build path is %build-path%
echo Client root is %client-root%

if exist %build-path%\shared-util_output (
	rmdir /s/q %build-path%\shared-util_output
	rem no error checking
)

echo checking for nupkg
if exist %build-root%\build_all\packaging\windows\*.nupkg (
	echo deleting nupkg
	del %build-root%\build_all\packaging\windows\*.nupkg
)

rem -----------------------------------------------------------------------------
rem -- build binaries
rem -----------------------------------------------------------------------------

mkdir %build-path%\shared-util_output
rem no error checking

call %build-root%\build_all\windows\build.cmd --make_nuget yes
if not %errorlevel%==0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Copy Win32 binaries
rem -----------------------------------------------------------------------------

rem -- Copy all Win32 files from cmake build directory to the repo directory
xcopy /q /y /R %build-path%\shared-util_Win32\Debug\*.* %build-path%\shared-util_output\win32\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Copy all Win32 Release files from cmake build directory to the repo directory
xcopy /q /y /R %build-path%\shared-util_Win32\Release\*.* %build-path%\shared-util_output\win32\Release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Copy x64 binaries
rem -----------------------------------------------------------------------------

rem -- Copy all x64 files from cmake build directory to the repo directory
xcopy /q /y /R %build-path%\shared-util_x64\Debug\*.* %build-path%\shared-util_output\x64\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Copy all x64 Release files from cmake build directory to the repo directory
xcopy /q /y /R %build-path%\shared-util_x64\Release\*.* %build-path%\shared-util_output\x64\Release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

rem -----------------------------------------------------------------------------
rem -- Copy x64 binaries
rem -----------------------------------------------------------------------------

rem -- Copy all x64 files from cmake build directory to the repo directory
xcopy /q /y /R %build-path%\shared-util_arm\Debug\*.* %build-path%\shared-util_output\arm\debug\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Copy all x64 Release files from cmake build directory to the repo directory
xcopy /q /y /R %build-path%\shared-util_arm\Release\*.* %build-path%\shared-util_output\arm\Release\*.*
if %errorlevel% neq 0 exit /b %errorlevel%

rem -- Package Nuget
nuget pack %build-root%\build_all\packaging\windows\Microsoft.Azure.C.SharedUtility.nuspec -OutputDirectory %build-root%\build_all\packaging\windows

rmdir %build-path%\shared-util_output /S /Q

echo done