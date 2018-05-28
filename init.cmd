@echo off

rem -- cd to repo directory if we are not already there
cd /d %~dp0


rem -- welcome message
echo ......................................................
echo .
echo .  Welcome to Carbon!

rem -- restore nuget packages
echo -
echo .  Installing Carbon dependent packages...
%~dp0\external\nuget\nuget.exe restore %~dp0\source\core\unidec\packages.config -PackagesDirectory %~dp0\packages 
echo .  Done!

rem -- special handling for Unidec
echo .
echo .  Updating Unidec files appropriately:
call %~dp0\init_unidec.cmd
echo .  Done!

rem -- goodbye message
echo .
echo .  Useful links:
echo .
echo .      http://aka.ms/carbon/wiki
echo .
echo ......................................................
