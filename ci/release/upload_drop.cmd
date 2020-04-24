Echo off
@Echo off
if "%1" equ "" (
  echo "Usage: %0 1.0.0"
  goto :EOF
)
Echo Version=%1
FOR /F %%f IN ('dir /q /b drop\*') DO (
call az storage blob upload --account-name csspeechstorage --container-name "drop" -f "drop\%%f" -n "%1/%%f"
)