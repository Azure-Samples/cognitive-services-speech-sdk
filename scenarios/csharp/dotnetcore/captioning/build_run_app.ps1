param(
    [string]$action
)

$dotnetPath = "C:\Program Files\dotnet"
$env:PATH = "$dotnetPath;$env:PATH"
$dotnetInstallationTempDirectory = "$env:TEMP\dotnet"
$dotnetTempPath = Join-Path $dotnetInstallationTempDirectory "dotnet.exe"

function Install-DotNet6 {
    Invoke-WebRequest -Uri https://dot.net/v1/dotnet-install.ps1 -OutFile dotnet-install.ps1
    if (-not $?) {
        Write-Host "Failed to download dotnet-install.ps1, exiting..." -ForegroundColor Red
        exit 1
    }

    & .\dotnet-install.ps1 -InstallDir $dotnetInstallationTempDirectory -Version 6.0.427
    if (-not $?) {
        Write-Host "Failed to install .NET SDK, exiting..." -ForegroundColor Red
        exit 1
    }

    Write-Host ".NET 6 installed successfully." -ForegroundColor Green
    Remove-Item -Force dotnet-install.ps1
}

function Test-GStreamer {
    if (-not (Get-Command gst-launch-1.0 -ErrorAction SilentlyContinue)) {
        Write-Host "GStreamer is not installed. Please install it before running the build." -ForegroundColor Yellow
        exit 1
    }
    Write-Host "GStreamer is installed." -ForegroundColor Green
}

if ($action -eq "build") {
    Test-GStreamer

    if (-not (Get-Command dotnet -ErrorAction SilentlyContinue) -or ([version]$(dotnet --version) -lt [version]"6.0")) {
        Write-Host "Installing .NET SDK 6.0..."

        Install-DotNet6

        & $dotnetTempPath add .\captioning package Microsoft.CognitiveServices.Speech --interactive --source https://api.nuget.org/v3/index.json
        if ($?) {
            Write-Host "Installation Microsoft.CognitiveServices.Speech package is succeeded." -ForegroundColor Green
        }
        else {
            Write-Host "Installation Microsoft.CognitiveServices.Speech package is failed, exiting..." -ForegroundColor Red
            exit 1
        }

        & $dotnetTempPath build .\captioning --configuration release
        if ($?) {
            Write-Host "Building is succeeded." -ForegroundColor Green
        }
        else {
            Write-Host "Building is failed, exiting..." -ForegroundColor Red
            exit 1
        }
    }
    else {
        & dotnet add .\captioning package Microsoft.CognitiveServices.Speech --interactive --source https://api.nuget.org/v3/index.json
        if ($?) {
            Write-Host "Installation Microsoft.CognitiveServices.Speech package is succeeded." -ForegroundColor Green
        }
        else {
            Write-Host "Installation Microsoft.CognitiveServices.Speech package is failed, exiting..." -ForegroundColor Red
            exit 1
        }

        & dotnet build .\captioning --configuration release
        if ($?) {
            Write-Host "Building is succeeded." -ForegroundColor Green
        }
        else {
            Write-Host "Building is failed, exiting..." -ForegroundColor Red
            exit 1
        }
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
        if (Get-Command dotnet -ErrorAction SilentlyContinue) {
            & dotnet run --project .\captioning\captioning.csproj --configuration release --realtime --input $inputFile
        }
        else {
            & $dotnetTempPath run --project .\captioning\captioning.csproj --configuration release --realtime --input $inputFile
        }
    } else {
        if (Get-Command dotnet -ErrorAction SilentlyContinue) {
            & dotnet run --project .\captioning\captioning.csproj --configuration release --realtime
        }
        else {
            & $dotnetTempPath run --project .\captioning\captioning.csproj --configuration release --realtime
        }
    }
}
else {
    Write-Host "Invalid action: $action" -ForegroundColor Red
    Write-Host "Usage: build or run"
    exit 1
}