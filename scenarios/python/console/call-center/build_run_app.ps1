param(
    [string]$action
)

if ($action -eq "build") {
    $pythonVersion = python --version 2>&1
    if (-not $pythonVersion -like "Python 3*") {
        Write-Host "Python 3 is not installed. Please install python3 first. Exiting..." -ForegroundColor Red
        exit 1
    }
}
elseif ($action -eq "run") {
    $configFilePath = "config.json"
    if (Test-Path $configFilePath) {
        $jsonContent = Get-Content -Path $configFilePath -Raw | ConvertFrom-Json
        $aiServiceKey = $jsonContent.SubscriptionKey
        $aiServiceRegion = $jsonContent.ServiceRegion
    }
    else {
        Write-Host "The config.json file is not found." -ForegroundColor Red
        exit 1
    }

    $pythonVersion = python --version 2>&1
    if ($pythonVersion -like "Python 3*") {
        & python .\call_center.py --speechKey $aiServiceKey --speechRegion $aiServiceRegion --languageKey $aiServiceKey --languageEndpoint "https://$aiServiceRegion.stt.speech.microsoft.com" --input "https://github.com/Azure-Samples/cognitive-services-speech-sdk/raw/master/scenarios/call-center/sampledata/Call1_separated_16k_health_insurance.wav" --output summary.json
    }
    else {
        Write-Host "Python 3 is not installed. Please install python3 first. Exiting..." -ForegroundColor Red
        exit 1
    }
}
else {
    Write-Host "Invalid action: $action" -ForegroundColor Red
    Write-Host "Usage: build or run"
    exit 1
}