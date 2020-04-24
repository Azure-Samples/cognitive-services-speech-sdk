@Echo off
if "%1" equ "" (
  echo "Usage: %0 1.0.0"
  goto :EOF
)
Echo Version=%1
Echo cleaning maven folder.
rd /s /q maven
mkdir maven

7z.exe e "zip\Android.zip" "Android\Android\maven\com\microsoft\cognitiveservices\speech\client-sdk\%1\*" -o"maven\com\microsoft\cognitiveservices\speech\client-sdk\%1\"
7z.exe e "zip\Android.zip" "Android\Android\maven\com\microsoft\cognitiveservices\speech\client-sdk-nomic\%1\*" -o"maven\com\microsoft\cognitiveservices\speech\client-sdk-nomic\%1\
7z.exe e "zip\JavaJrePackage.zip" "JavaJrePackage\maven\com\microsoft\cognitiveservices\speech\client-sdk\%1\*" -o"maven\com\microsoft\cognitiveservices\speech\client-sdk\%1\
7z.exe e "zip\RemoteConversationJar.zip" "RemoteConversationJar\maven\com\microsoft\cognitiveservices\speech\remoteconversation\remote-conversation\%1\*" -o"maven\com\microsoft\cognitiveservices\speech\remoteconversation\remote-conversation\%1\"