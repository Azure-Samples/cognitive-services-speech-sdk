param(
    [string]$action
)

function Test-PythonInstalled {
    return Get-Command python -ErrorAction SilentlyContinue
}

function Test-PipInstalled {
    return Get-Command pip -ErrorAction SilentlyContinue
}

if ($action -eq "build") {
    if (-not (Test-PythonInstalled)) {
        Write-Host "Python is not installed. Please install Python to proceed." -ForegroundColor Red
        exit 1
    }

    if (-not (Test-PipInstalled)) {
        Write-Host "pip is not installed. Please install pip to proceed." -ForegroundColor Red
        exit 1
    }

    Write-Host "Installing azure-cognitiveservices-speech package..."
    try {
        pip install azure-cognitiveservices-speech
        Write-Host "Package installation succeeded." -ForegroundColor Green
    }
    catch {
        Write-Host "Package installation failed. Please check your pip installation." -ForegroundColor Red
        exit 1
    }
}
elseif ($action -eq "run") {
    $configFilePath = "config.json"
    if (Test-Path $configFilePath) {
        $configContent = Get-Content -Raw -Path $configFilePath | ConvertFrom-Json
    
        $subscriptionKey = $configContent.SubscriptionKey
        $serviceRegion = $configContent.ServiceRegion

        if ($subscriptionKey) {
            [System.Environment]::SetEnvironmentVariable("SPEECH_KEY", $subscriptionKey)
        }
    
        if ($serviceRegion) {
            [System.Environment]::SetEnvironmentVariable("SPEECH_REGION", $serviceRegion)
        }

        Write-Host "Environment variables loaded from $configFilePath"
    }
    else {
        Write-Host "File not found: $configFilePath"
    }

    $inputFile = Read-Host "Please enter the path to the input .wav file (press Enter to use the default microphone)"
    if ([string]::IsNullOrEmpty($inputFile)) {
        & python .\captioning.py
    } else {
        & python .\captioning.py --input $inputFile
    }
}
else {
    Write-Host "Invalid action: $action" -ForegroundColor Red
    Write-Host "Usage: -action build or -action run"
    exit 1
}
