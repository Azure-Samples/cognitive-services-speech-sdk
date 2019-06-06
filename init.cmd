@echo off

rem -- cd to repo directory if we are not already there
cd /d %~dp0


rem -- welcome message
echo ......................................................
echo .
echo .  Welcome to Carbon!

rem -- special handling for Unidec
echo .
echo .  Adding Unidec binaries to path...
set PATH=%PATH%;%~dp0\external\unidec\Richland.Speech.UnidecRuntime\native
echo .  Done!

rem -- goodbye message
echo .
echo .  Useful links:
echo .
echo .      http://aka.ms/carbon/wiki
echo .
echo ......................................................
