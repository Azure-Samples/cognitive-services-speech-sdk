@echo off

rem -- cd to repo directory if we are not already there
cd /d %~dp0

rem -- what build flavor are we using...
set _BUILD_FLAVOR=%1
if "%_BUILD_FLAVOR%"=="" (set _BUILD_FLAVOR=Debug&&echo .  -  Using DEFAULT build flavor DEBUG ... )

rem -- ensure appropriate paths are in place
if not exist %~dp0\external\unidec\include md %~dp0\external\unidec\include
if not exist %~dp0\external\unidec\lib md %~dp0\external\unidec\lib
if not exist %~dp0\external\unidec\bin md %~dp0\external\unidec\bin
if not exist %~dp0\build\bin\%_BUILD_FLAVOR% md %~dp0\build\bin\%_BUILD_FLAVOR%

rem -- copy unidec executable binaries in place
set _UNIDEC_VERSION=Richland.Speech.UnidecRuntime.18.1.5.24286743

echo .  -  Updating headers/libraries ...
xcopy /D %~dp0\packages\%_UNIDEC_VERSION%\build\native\*.h %~dp0\external\unidec\include >nul
xcopy /D %~dp0\packages\%_UNIDEC_VERSION%\build\native\*.lib %~dp0\external\unidec\lib >nul

echo .  -  Updating headers/executables ...
xcopy /Y /D %~dp0\packages\%_UNIDEC_VERSION%\build\native\*.dll %~dp0\build\bin\%_BUILD_FLAVOR% >nul
xcopy /Y /D %~dp0\packages\%_UNIDEC_VERSION%\build\native\*.pdb %~dp0\build\bin\%_BUILD_FLAVOR% >nul
xcopy /Y /D %~dp0\packages\%_UNIDEC_VERSION%\build\native\*.exe %~dp0\build\bin\%_BUILD_FLAVOR% >nul
xcopy /Y /D %~dp0\packages\%_UNIDEC_VERSION%\lib\net\*.dll %~dp0\build\bin\%_BUILD_FLAVOR% >nul
xcopy /Y /D %~dp0\packages\%_UNIDEC_VERSION%\lib\net\*.pdb %~dp0\build\bin\%_BUILD_FLAVOR% >nul

echo .  -  Updating models (this might take several minutes) ...
xcopy /Y /D \\ccpsofslm\speechshare\USR\carbon\richland\models %~dp0\external\unidec\bin /S >nul
