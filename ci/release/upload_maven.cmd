if "%1" equ "" (
  echo "Usage: %0 1.0.0"
  goto :EOF
)
echo "Version=%1"
call az storage blob upload --account-name csspeechstorage --container-name "maven" -f "maven\com\microsoft\cognitiveservices\speech\client-sdk\%1\client-sdk-%1.aar" -n "com\microsoft\cognitiveservices\speech\client-sdk\%1\client-sdk-%1.aar"
call az storage blob upload --account-name csspeechstorage --container-name "maven" -f "maven\com\microsoft\cognitiveservices\speech\client-sdk\%1\client-sdk-%1.jar" -n "com\microsoft\cognitiveservices\speech\client-sdk\%1\client-sdk-%1.jar"
call az storage blob upload --account-name csspeechstorage --container-name "maven" -f "maven\com\microsoft\cognitiveservices\speech\client-sdk\%1\client-sdk-%1.pom" -n "com\microsoft\cognitiveservices\speech\client-sdk\%1\client-sdk-%1.pom"
call az storage blob upload --account-name csspeechstorage --container-name "maven" -f "maven\com\microsoft\cognitiveservices\speech\client-sdk\%1\client-sdk-%1.aar" -n "com\microsoft\cognitiveservices\speech\client-sdk-nomic\%1\client-sdk-nomic-%1.aar"
call az storage blob upload --account-name csspeechstorage --container-name "maven" -f "maven\com\microsoft\cognitiveservices\speech\client-sdk\%1\client-sdk-%1.pom" -n "com\microsoft\cognitiveservices\speech\client-sdk-nomic\%1\client-sdk-nomic-%1.pom"
call az storage blob upload --account-name csspeechstorage --container-name "maven" -f "maven\com\microsoft\cognitiveservices\speech\remoteconversation\remote-conversation\%1\remote-conversation-%1.jar" -n "com\microsoft\cognitiveservices\speech\remoteconversation\remote-conversation\%1\remote-conversation-%1.jar"
call az storage blob upload --account-name csspeechstorage --container-name "maven" -f "maven\com\microsoft\cognitiveservices\speech\remoteconversation\remote-conversation\%1\remote-conversation-%1.pom" -n "com\microsoft\cognitiveservices\speech\remoteconversation\remote-conversation\%1\remote-conversation-%1.pom"
