@setlocal
@echo off

REM find current branch
for /f "usebackq tokens=*" %%i in (`git symbolic-ref -q HEAD`) do set "current_branch_ref=%%i"
if defined current_branch_ref set "current_branch=%current_branch_ref:refs/heads/=%"

REM handle detached HEAD
if not defined current_branch (
    echo You're not on any branch! Aborting...
    goto :eof
)

REM Don't rewrite history on 'develop' or 'master' branches
set __exit=1
if not "%current_branch%"=="master" if not "%current_branch%"=="develop" set __exit=0
if %__exit%==1 (
    echo You cannot call this script from 'develop' or 'master'. Change to a topic branch first.  Aborting...
    goto :eof
)

REM find tracking branch
for /f "usebackq tokens=*" %%i in (`git rev-parse --symbolic-full-name --abbrev-ref @{u}`) do set "tracking_branch=%%i"
if not defined tracking_branch (
    echo Branch ^'%current_branch%^' is not tracking a remote branch! Aborting...
    goto :eof
)

for /f "usebackq tokens=*" %%i in (`git log --oneline -1`) do set "head_commit=%%i"
choice /C yn /M "Rewrite HEAD commit (%head_commit%) on branch '%current_branch%', and push to '%tracking_branch%'?"

if not %errorlevel%==1 goto :eof

for /f "usebackq tokens=*" %%i in (`git config branch.%current_branch%.remote`) do set "remote=%%i"
if not defined remote (
    REM should never happen...
    echo Error: could not isolate remote name for tracking branch ^'%tracking_branch%^'! Aborting...
    goto :eof
)

for /f "usebackq tokens=*" %%i in (`call echo %%tracking_branch:%remote%/^=%%`) do set "tracking_abbrev=%%i"

git commit --amend --no-edit
git push -f %remote% %current_branch%:%tracking_abbrev%
