@Echo off
if "%1" equ "" (
  echo "Usage: %0 1.0.0"
  goto :EOF
)

call npm publish npm\microsoft-cognitiveservices-speech-sdk-%1.tgz --dry-run

if NOT %ERRORLEVEL% equ 0 (
    echo ""
    echo ""
    echo "Aborting due to error during dry run (%ERRORLEVEL%)."
    goto :EOF
)
echo "no obvious errors found in dry run, run this for real? (ctrl+c to abort)"
pause
call npm login
call npm publish npm\microsoft-cognitiveservices-speech-sdk-%1.tgz