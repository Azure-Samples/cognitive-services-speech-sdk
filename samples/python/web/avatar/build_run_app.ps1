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

    Write-Host "Installing requirements packages..."
    try {
        pip install -r requirements.txt
        Write-Host "Requirements packages installation succeeded." -ForegroundColor Green
    }
    catch {
        Write-Host "Requirements packages installation failed. Please check your pip installation." -ForegroundColor Red
        exit 1
    }
}
elseif ($action -eq "run") {
    $configFilePath = "config.json"
    if (Test-Path $configFilePath) {
        $configContent = Get-Content -Raw -Path $configFilePath | ConvertFrom-Json
    
        $subscriptionKey = $configContent.SubscriptionKey
        $serviceRegion = $configContent.ServiceRegion
        $resourceName = $configContent.ResourceName
        $endpoint = "https://$resourceName.openai.azure.com/"

        if ($subscriptionKey) {
            [System.Environment]::SetEnvironmentVariable("SPEECH_KEY", $subscriptionKey)
            [System.Environment]::SetEnvironmentVariable("AZURE_OPENAI_API_KEY", $subscriptionKey)
        }
    
        if ($serviceRegion) {
            [System.Environment]::SetEnvironmentVariable("SPEECH_REGION", $serviceRegion)
        }

        if($endpoint){
            [System.Environment]::SetEnvironmentVariable("AZURE_OPENAI_ENDPOINT", $endpoint)
        }

        Write-Host "Environment variables loaded from $configFilePath"
    }
    else {
        Write-Host "File not found: $configFilePath"
    }

    & python -m flask run -h 0.0.0.0 -p 5000
}
else {
    Write-Host "Invalid action: $action" -ForegroundColor Red
    Write-Host "Usage: -action build or -action run"
    exit 1
}
