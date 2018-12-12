@REM Copyright (c) Microsoft Corporation. All rights reserved.
@REM Licensed under the MIT license.
@echo off
setlocal
if "%~1" equ "/?" goto :Usage
if "%~1" equ "-?" goto :Usage
if "%~1" equ "?" goto :Usage
if "%~1" equ "/h" goto :Usage
if "%~1" equ "-h" goto :Usage

set TEST_SETTING_FILE_DIR=%~dp0\secrets
if NOT EXIST "%TEST_SETTING_FILE_DIR%" (
    md "%TEST_SETTING_FILE_DIR%" || (
        echo Error creating directory %TEST_SETTING_FILE_DIR%
        exit /b 1
    )
)
 
set TEST_SETTING_FILE_NAME=%TEST_SETTING_FILE_DIR%\TestConfiguration.ts

if EXIST "%TEST_SETTING_FILE_NAME%" (
    echo Clearing values from settings file.
    echo. > "%TEST_SETTING_FILE_NAME%" || (
        echo Error creating file %TEST_SETTING_FILE_NAME%
        exit /b 1
    )
)

@echo import { Settings } from "../tests/Settings" > "%TEST_SETTING_FILE_NAME%"

:NextArg
if "%~1" == "" (
    goto :eof
)

for /f "tokens=1,2 delims=:" %%I in ("%~1") do (
    echo Setting Settings.%%I = "%%J"
    echo Settings.%%I = "%%J"; >> "%TEST_SETTING_FILE_NAME%"
)

shift
goto :NextArg

exit /b 0

:Usage
@echo off
echo.
echo Usage: %~n0 ^<ParamName^>:^<Value^> 
echo.
echo Writes any ^<ParamName^>:^<Value^> pair to the test settings file for JavaScript bindings tests.
echo.
echo The file will be erased before new values are added.
echo.
echo Current settings available are: [SpeechSubscriptionKey:^<key^>] [SpeechRegion:^<region^>] [LuisSubscriptionKey:^<LuisKey^>]  [LuisRegion:^<region^>] [LuisAppId:^<LuisAppId^>]
echo.
exit /b 1
