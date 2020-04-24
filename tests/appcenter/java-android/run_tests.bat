@echo off
REM
REM Copyright (c) Microsoft. All rights reserved.
REM Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
REM
setlocal

REM override these settings by using environment variables, if necessary
if "%APPCENTER_APPID%"     EQU "" set APPCENTER_APPID=SpeechSDKTester
if "%APPCENTER_TESTQUEUE%" EQU "" set APPCENTER_TESTQUEUE=local
if "%APPCENTER_DEVICESET%" EQU "" set APPCENTER_DEVICESET=deviceset-dev:1
if "%APPCENTER_USER_ID%"   EQU "" (
    echo Missing: set APPCENTER_USER_ID=YOUR_APPCENTER_ID
    exit /b 1
)

echo.
echo Using these App Center settings:
echo   set APPCENTER_USER_ID=%APPCENTER_USER_ID%
echo   set APPCENTER_APPID=%APPCENTER_APPID%
echo   set APPCENTER_TESTQUEUE=%APPCENTER_TESTQUEUE%
echo   set APPCENTER_DEVICESET=%APPCENTER_DEVICESET%


set SPEECHSDK_TEST_ROOT=%~dp0
set SPEECHSDK_ROOT=%~dp0\..\..\..\


set SPEECHSDK_BUILD_ROOT=%~dpf1
set SPEECHSDK_BUILD_LIB="%SPEECHSDK_BUILD_ROOT%\lib"
set SPEECHSDK_BUILD_BIN="%SPEECHSDK_BUILD_ROOT%\bin"

REM all updating is relative to this script, so go there.
cd /d "%~dp0"
if errorlevel 1 (
    echo Could not cd to script directory.
    exit /b 1
)

echo Now cleaning up the test app
call git clean -dxf .
if errorlevel 1 (
    echo Could not cleanup script and tests.
    exit /b 1
)

echo.
if NOT EXIST "%SPEECHSDK_BUILD_LIB%"\libMicrosoft.CognitiveServices.Speech.core.so (
    echo Could not find the SpeechSdk core library. Expecting it at
    echo     "%SPEECHSDK_BUILD_LIB%"\libMicrosoft.CognitiveServices.Speech.core.so
    exit /b 1
)

echo Found Carbon native libraries (these are needed until we use the AAR library for android builds directly)
echo     bin directory "%SPEECHSDK_BUILD_BIN%"
echo     lib directory "%SPEECHSDK_BUILD_LIB%"

if NOT EXIST "%SPEECHSDK_BUILD_ROOT%\MainActivity.properties" (
    echo You need a file "%SPEECHSDK_BUILD_ROOT%"\MainActivity.properties with the subscription keys and other settings.
    echo Put in the following data:
    echo.
    echo SpeechSubscriptionKey = YourSubscriptionKey
    echo SpeechRegion = YourRegion
    echo LuisSubscriptionKey = enter your subscription info here
    echo LuisRegion = YourLuisRegion
    echo LuisAppId = enter your LUIS AppId
    echo Keyword = Computer
    echo KeywordModel = kws-computer.zip
    echo Keyword = Computer
    echo KeywordModel = kws-computer.zip
    echo SasToken = YourSasToken
    echo SasContainerPrefix = uploadtest/testrun
    echo.
    exit /b 1
)

echo.
echo Copying audio test files to test application
if not exist "%SPEECHSDK_TEST_ROOT%\app\src\main\assets\" mkdir "%SPEECHSDK_TEST_ROOT%\app\src\main\assets" && ^
powershell -NoProfile -NoLogo -ExecutionPolicy Bypass -NonInteractive Add-Type -Assembly System.IO.Compression.FileSystem; $compressionLevel = [System.IO.Compression.CompressionLevel]::Optimal; [System.IO.Compression.ZipFile]::CreateFromDirectory('%SPEECHSDK_ROOT%\tests\input\audio', '%SPEECHSDK_TEST_ROOT%\app\src\main\assets\audiotestassets.zip', $compressionLevel, $false) && ^
powershell -NoProfile -NoLogo -ExecutionPolicy Bypass -NonInteractive Add-Type -Assembly System.IO.Compression.FileSystem; $compressionLevel = [System.IO.Compression.CompressionLevel]::Optimal; [System.IO.Compression.ZipFile]::CreateFromDirectory('%SPEECHSDK_ROOT%\tests\input\kws', '%SPEECHSDK_TEST_ROOT%\app\src\main\assets\kwstestassets.zip', $compressionLevel, $false) && ^
copy /Y "%SPEECHSDK_BUILD_ROOT%\MainActivity.properties"                 "%SPEECHSDK_TEST_ROOT%\app\src\main\assets\" && ^
copy /Y "%SPEECHSDK_ROOT%\tests\test.defaults.json"                "%SPEECHSDK_TEST_ROOT%\app\src\main\assets\" && ^
copy /Y "%SPEECHSDK_ROOT%\tests\test.subscriptions.regions.json"   "%SPEECHSDK_TEST_ROOT%\app\src\main\assets\" && ^
copy /Y "%SPEECHSDK_ROOT%\tests\test.audio.utterances.json"        "%SPEECHSDK_TEST_ROOT%\app\src\main\assets\"
if errorlevel 1 (
    echo Could not copy test data.
    exit /b 1
)

echo.
echo Copying Speech Sdk libs to test application
md "%SPEECHSDK_TEST_ROOT%\app\src\main\jniLibs\armeabi" && ^
copy /Y "%SPEECHSDK_BUILD_LIB%\com.microsoft.cognitiveservices.speech.jar"             "%SPEECHSDK_TEST_ROOT%\app\src\main\jniLibs\"          && ^
copy /Y "%SPEECHSDK_BUILD_BIN%\com.microsoft.cognitiveservices.speech.tests.jar"       "%SPEECHSDK_TEST_ROOT%\app\src\main\jniLibs\"          && ^
copy /Y "%SPEECHSDK_BUILD_LIB%\libMicrosoft.CognitiveServices.Speech.core.so"          "%SPEECHSDK_TEST_ROOT%\app\src\main\jniLibs\armeabi\"  && ^
copy /Y "%SPEECHSDK_BUILD_BIN%\libMicrosoft.CognitiveServices.Speech.java.bindings.so" "%SPEECHSDK_TEST_ROOT%\app\src\main\jniLibs\armeabi\"
if errorlevel 1 (
    echo Could not copy necessary native libs.
    exit /b 1
)
if EXIST "%SPEECHSDK_BUILD_LIB%\libMicrosoft.CognitiveServices.Speech.extension.kws.so" (
    copy /Y "%SPEECHSDK_BUILD_LIB%\libMicrosoft.CognitiveServices.Speech.extension.kws.so" "%SPEECHSDK_TEST_ROOT%\app\src\main\jniLibs\armeabi\"
    if errorlevel 1 (
        echo Could not copy kws extension.
        exit /b 1
    )
)


echo.
echo Now building app and tester with gradle.
call gradlew.bat build assembleDebugAndroidTest
if errorlevel 1 (
    echo Could not build the test app.
    exit /b 1
)

echo.
echo Now submitting executable to App Center for testing
call appcenter test run espresso %APPCENTER_TOKEN_OPTION% ^
        --app         "%APPCENTER_USER_ID%/%APPCENTER_APPID%" ^
        --devices     "%APPCENTER_USER_ID%/%APPCENTER_DEVICESET%" ^
        --app-path    "%SPEECHSDK_TEST_ROOT%\app\build\outputs\apk\debug\app-debug.apk" ^
        --test-series %APPCENTER_TESTQUEUE% ^
        --locale      "en_US" ^
        --test-parameter package=com.microsoft.cognitiveservices.speech.samples.sdsdkstarterapp ^
        --build-dir   "%SPEECHSDK_TEST_ROOT%\app\build\outputs\apk\androidTest\debug"
