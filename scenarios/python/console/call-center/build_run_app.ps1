param(
    [string]$action
)

$pythonPath = Get-Command python -ErrorAction SilentlyContinue
$env:Path = "$pythonPath;$env:Path"
$tempPythonInstallationDirectory = "$env:LOCALAPPDATA\Python310"
$tempPythonPath = Join-Path $tempPythonInstallationDirectory "python.exe"

function Test-Python3Installed {
    if ($pythonPath) {
        $pythonVersion = python --version 2>&1
        if ($pythonVersion -like "Python 3*") {
            return $true
        }
    }
    return $false
}

if ($action -eq "build") {
    if (-not (Test-Python3Installed)) {
        Write-Host "Python 3 is not installed. Installing Python 3 to $tempPythonInstallationDirectory..."

        New-Item -ItemType Directory -Force -Path $tempPythonInstallationDirectory

        if ([Environment]::Is64BitOperatingSystem) {
            $pythonDownloadUrl = "https://www.python.org/ftp/python/3.10.0/python-3.10.0-amd64.exe"
        }
        else {
            $pythonDownloadUrl = "https://www.python.org/ftp/python/3.10.0/python-3.10.0-win32.exe"
        }
        $pythonInstallerPath = ".\python-installer.exe"
        Invoke-WebRequest -Uri $pythonDownloadUrl -OutFile $pythonInstallerPath
        if (-not $?) {
            Write-Host "Failed to download python3, exiting..." -ForegroundColor Red
            exit 1
        }

        Start-Process -FilePath $pythonInstallerPath -ArgumentList "/quiet InstallAllUsers=0 PrependPath=0 TargetDir=$tempPythonInstallationDirectory" -Wait
        if (-not (Get-Command $tempPythonPath -ErrorAction SilentlyContinu)) {
            Write-Host "Python3 installation failed, exiting..." -ForegroundColor Red
            exit 1
        }

        Remove-Item -Force -Path $pythonInstallerPath
    }
}
elseif ($action -eq "run") {
    if ($pythonPath) {
        & python .\call_center.py --speechKey *** --speechRegion *** --languageKey *** --languageEndpoint *** --input sample.wav --output out.txt
    }
    elseif (Get-Command $tempPythonPath -ErrorAction SilentlyContinue) {
        & $tempPythonPath .\call_center.py --speechKey *** --speechRegion *** --languageKey *** --languageEndpoint *** --input sample.wav --output out.txt
    }
    else {
        Write-Host "Python is not found. Please first run the script with build action to install Python." -ForegroundColor Red
        exit 1
    }
}
else {
    Write-Host "Invalid action: $action" -ForegroundColor Red
    Write-Host "Usage: build or run"
    exit 1
}