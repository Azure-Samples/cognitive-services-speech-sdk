@Echo off
if "%1" equ "" (
  echo "Usage: %0 1.0.0"
  goto :EOF
)
Echo Version=%1
Echo cleaning drop folder.
rd /s /q drop
mkdir drop

echo extracting needed files from windows.zip
7z.exe e "zip\windows.zip" "Windows\Microsoft.CognitiveServices.Speech.%1.nupkg" -o"drop\"
7z.exe e "zip\windows.zip" "Windows\Microsoft.CognitiveServices.Speech.Xamarin.iOS.%1.nupkg" -o"drop\"
7z.exe e "zip\windows.zip" "Windows\Microsoft.CognitiveServices.Speech.Remoteconversation.%1.nupkg" -o"drop\"

echo extracting needed files from unity.zip
7z.exe e "zip\Unity.zip" "Unity\Microsoft.CognitiveServices.Speech.%1.unitypackage" -o"drop\"

echo extracting needed files from iOS.zip
7z.exe e "zip\iOS.zip" "iOS\release\MicrosoftCognitiveServicesSpeech-iOS-%1.zip" -o"drop\"

echo extracting needed files from macOS.zip
7z.exe e "zip\macOS.zip" "macOS\release\MicrosoftCognitiveServicesSpeech-macOS-%1.zip" -o"drop\"

echo extracting needed files from Linux.zip
7z.exe e "zip\Linux.zip" "Linux\Drop\SpeechSDK-Linux-%1.tar.gz" -o"drop\"

echo extracting needed files from JavaScript.zip
7z.exe e "zip\JavaScript.zip" "JavaScript\SpeechSDK-JavaScript-%1.zip" -o"drop\"
