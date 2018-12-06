@REM Copyright (c) Microsoft Corporation. All rights reserved.
@REM Licensed under the MIT license.
@echo off
setlocal
if "%~1" equ "/?" goto :Usage
if "%~1" equ "-?" goto :Usage
if "%~1" equ "?" goto :Usage
if "%~1" equ "/h" goto :Usage
if "%~1" equ "-h" goto :Usage

set TEST_SETTING_FILE_NAME=%~dp0\secrets\TestConfiguration.ts

set TEST_SETTING_FILE_EXISTED=0
if EXIST "%TEST_SETTING_FILE_NAME%" set TEST_SETTING_FILE_EXISTED=1

if "%~1" NEQ "" (
    call "%~dp0BuildTestConfig.cmd" %* || (
        echo Error creating test config.
        exit /b 1
    )
) else if %TEST_SETTING_FILE_EXISTED% EQU 0 (
  echo Warning: No test config and no parameters specified. This will probably fail. 1>&2
)

pushd "%~dp0"
call npm run test

set NPM_ERROR=%ERRORLEVEL%

if %TEST_SETTING_FILE_EXISTED% EQU 0 (
    del "%TEST_SETTING_FILE_NAME%"
)

popd

exit /b %NPM_ERROR%

@REM TODO fix usage
:Usage
@echo off
echo.
echo Usage: %~n0 [^<ParamName^>:^<Value^>]
echo.
echo Writes any ^<ParamName^>:^<Value^> pair to the test settings file for JavaScript bindings tests.
echo.
echo The file will be erased before new values are added.
echo.
echo If no values are specified, the existing file will not be modified.
echo.
echo Current settings available are: [SpeechSubscriptionKey:^<key^>] [SpeechAuthorizationToken:^<SpeechAuthorizationToken^>] [SpeechRegion:^<region^>] [LuisSubscriptionKey:^<LuisKey^>]  [LuisRegion:^<region^>] [LuidAppId:^<LuisAppId^>]
echo.
echo Once settings are written, executes the JS unit tests and if the settings file was created, it will be deleted.
exit /b 1
