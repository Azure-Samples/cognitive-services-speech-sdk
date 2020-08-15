@echo off

rem -- cd to repo directory if we are not already there
cd /d %~dp0


rem -- welcome message
echo ......................................................
echo .
echo .  Welcome to Carbon!

rem -- special handling for offline features
echo .
echo .  Adding offline engine binaries to path...
rem Unidec
set PATH=%PATH%;%~dp0\external\unidec\Richland.Speech.UnidecRuntime\native
rem RNN-T
set PATH=%PATH%;%~dp0\external\mas\Windows\x64\Release
rem TTS
set PATH=%PATH%;%~dp0\external\offline_tts\Windows\x64
echo .  Done!

rem -- goodbye message
echo .
echo .  Useful links:
echo .
echo .      http://aka.ms/carbon/wiki
echo .
echo ......................................................
