if "%1" equ "" (
  echo "Usage: %0 1.0.0"
  goto :EOF
)
echo "Version=%1"
call az storage blob upload --account-name csspeechstorage --container-name "drop" -f "drop\SpeechSDK-JavaScript-%1.zip" -n "%1\SpeechSDK-JavaScript-%1.zip"
