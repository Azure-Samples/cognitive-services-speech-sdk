@Echo off
if "%1" equ "" (
  echo "Usage: %0 1.0.0"
  goto :EOF
)
Echo Version=%1

echo extracting SpeechSDK-JavaScript-%1.zip from JavaScript.zip to DROP folder
7z.exe e "zip\JavaScript.zip" "JavaScript\SpeechSDK-JavaScript-%1.zip" -o"drop\"

echo extracting needed files from JavaScript.zip
7z.exe e "zip\JavaScript.zip" "JavaScript\npm\microsoft-cognitiveservices-speech-sdk-%1.tgz" -o"npm\"
