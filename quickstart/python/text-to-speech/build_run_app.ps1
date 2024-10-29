param(
    [string]$action
)

if ($action -eq "build") {
    if (!(Get-Command "python" -ErrorAction SilentlyContinue)) {
        Write-Host "Python is not installed. Please install python first. Exiting..." -ForegroundColor Red
        exit 1
    }

    if (!(Get-Command "pip" -ErrorAction SilentlyContinue)) {
        Write-Host "pip is not installed. Please install pip first. Exiting..." -ForegroundColor Red
        exit 1
    }

    & pip install azure-cognitiveservices-speech
    if (! $?) {
        Write-Host "The azure-cognitiveservices-speech package failed to install. Exiting..." -ForegroundColor Red
        exit 1
    }
}
elseif ($action -eq "run") {
    if (Get-Command "python" -ErrorAction SilentlyContinue) {
        & python quickstart.py
    }
    else {
        Write-Host "Python is not installed. Please install python first. Exiting..." -ForegroundColor Red
        exit 1
    }
}
else {
    Write-Host "Invalid action: $action" -ForegroundColor Red
    Write-Host "Usage: build or run"
    exit 1
}
