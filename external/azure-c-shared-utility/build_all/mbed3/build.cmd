@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

@REM  Script arguments:

@REM  %1 File list for the project that is being built

set project_path=..\..
set filelist=shared-util_filelist.txt

if exist source rd /s /q source

mkdir source

echo Copying files ...

for /f "tokens=*" %%i in (%filelist%) DO (
    xcopy /y "%project_path%\%%i" "source"
    if %errorlevel% neq 0 goto :eof
)
echo .

yotta target frdm-k64f-gcc
if %errorlevel% neq 0 goto :eof

yotta build
if %errorlevel% neq 0 goto :eof
