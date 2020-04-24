@Echo off
Echo cleaning wheelhouse folder.
rd /s /q wheelhouse
mkdir wheelhouse

echo extracting wheel files from windows.zip
7z.exe e "zip\windows.zip" "Windows\x64\Release\wheelhouse\*" -o"wheelhouse\"
7z.exe e "zip\windows.zip" "Windows\win32\Release\wheelhouse\*" -o"wheelhouse\"

echo extracting wheel files from linux.zip
7z.exe e "zip\linux.zip" "Linux\Linux-x64\Release\wheelhouse\*" -o"wheelhouse\"

echo extracting wheel files from macOS.zip
7z.exe e "zip\macOS.zip" "macOS\Release\wheelhouse\*" -o"wheelhouse\"