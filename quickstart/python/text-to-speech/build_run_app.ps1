param(
    [string]$action
)

$pythonPath = Get-Command python -ErrorAction SilentlyContinue
$env:Path = "$pythonPath;$env:Path"
$tempPythonInstallationDirectory = "$env:TEMP\Python310"
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

function Install-Packages {
    param (
        [string]$pythonDirectory
    )

    $realPythonPath = Join-Path $pythonDirectory "python.exe"
    try {
        & $realPythonPath -m pip install azure-cognitiveservices-speech
    }
    catch {
        Write-Host "The pip is not installed. Installing pip..."

        Invoke-WebRequest -Uri "https://bootstrap.pypa.io/get-pip.py" -OutFile "$pythonDirectory\get-pip.py"
        if (-not $?) {
            Write-Host "Failed to download pip, exiting..." -ForegroundColor Red
            exit 1
        }
        & $realPythonPath "$pythonDirectory\get-pip.py" --no-warn-script-location --prefix $pythonDirectory
        if (-not $?) {
            Write-Host "The pip installation failed, exiting..." -ForegroundColor Red
            exit 1
        }

        Remove-Item -Force -Path "$pythonDirectory\get-pip.py"

        & $realPythonPath -m pip install azure-cognitiveservices-speech
        if (-not $?) {
            Write-Host "The azure-cognitiveservices-speech package installation failed, exiting..." -ForegroundColor Red
            exit 1
        }
    }
}

if ($action -eq "build") {
    if (Test-Python3Installed) {
        $pythonDirectory = Split-Path $pythonPath.Path
        Install-Packages -pythonDirectory $pythonDirectory
    }
    elseif (-not (Get-Command $tempPythonPath -ErrorAction SilentlyContinue)) {
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

        Install-Packages -pythonDirectory $tempPythonInstallationDirectory
    }
    else {
        Install-Packages -pythonDirectory $tempPythonInstallationDirectory
    }
}
elseif ($action -eq "run") {
    if ($pythonPath) {
        & python .\quickstart.py
    }
    elseif (Get-Command $tempPythonPath -ErrorAction SilentlyContinue) {
        & $tempPythonPath .\quickstart.py
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
