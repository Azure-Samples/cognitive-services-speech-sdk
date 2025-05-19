param(
    [string]$action
)

$dotnetPath = "C:\Program Files\dotnet"
$env:PATH = "$dotnetPath;$env:PATH"
$dotnetInstallationTempDirectory = "$env:LOCALAPPDATA\dotnet"
$dotnetTempPath = Join-Path $dotnetInstallationTempDirectory "dotnet.exe"

function Install-DotNet8 {
    Invoke-WebRequest -Uri https://dot.net/v1/dotnet-install.ps1 -OutFile dotnet-install.ps1
    if (-not $?) {
        Write-Host "Failed to download dotnet-install.ps1, exiting..." -ForegroundColor Red
        exit 1
    }

    & .\dotnet-install.ps1 -InstallDir $dotnetInstallationTempDirectory -Version 8.0.405
    if (-not $?) {
        Write-Host "Failed to install .NET SDK, exiting..." -ForegroundColor Red
        exit 1
    }

    Write-Host ".NET 8 installed successfully." -ForegroundColor Green
    Remove-Item -Force dotnet-install.ps1
}

if ($action -eq "build") {
    if (-not (Get-Command dotnet -ErrorAction SilentlyContinue) -or ([version]$(dotnet --version) -lt [version]"8.0")) {
        Write-Host "Installing .NET SDK 8.0..."

        Install-DotNet8

        & $dotnetTempPath build .\call-center --configuration release
        if ($?) {
            Write-Host "Building is succeeded." -ForegroundColor Green
        }
        else {
            Write-Host "Building is failed, exiting..." -ForegroundColor Red
            exit 1
        }
    }
    else {
        & dotnet build .\call-center --configuration release
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
        $jsonContent = Get-Content -Path $configFilePath -Raw | ConvertFrom-Json
        $aiServiceKey = $jsonContent.SubscriptionKey
        $aiServiceRegion = $jsonContent.ServiceRegion
        $customSubDomainName = $jsonContent.CustomSubDomainName
    }
    else {
        Write-Host "The config.json file is not found." -ForegroundColor Red
        exit 1
    }

    $projectPath = ".\call-center\call-center.csproj"
    $commandLineArgs = @(
        "--project", $projectPath,
        "--speechKey", $aiServiceKey,
        "--speechRegion", $aiServiceRegion,
        "--languageKey", $aiServiceKey,
        "--languageEndpoint", "https://$customSubDomainName.cognitiveservices.azure.com/",
        "--input", "https://github.com/Azure-Samples/cognitive-services-speech-sdk/raw/master/scenarios/call-center/sampledata/Call1_separated_16k_health_insurance.wav",
        "--output", "summary.json",
        "--configuration", "release",
        "--stereo"
    )
    if (Get-Command dotnet -ErrorAction SilentlyContinue) {
        Write-Host "Running command: dotnet run $($commandLineArgs -join ' ')" -ForegroundColor Green
        & dotnet run $commandLineArgs
    }
    elseif (Get-Command $dotnetTempPath -ErrorAction SilentlyContinue) {
        Write-Host "Running command: $dotnetTempPath run $($commandLineArgs -join ' ')" -ForegroundColor Green
        & $dotnetTempPath run $commandLineArgs
    }
    else {
        Write-Host ".NET SDK is not found. Please first run the script with build action to install .NET 8.0." -ForegroundColor Red
        exit 1
    }
}
else {
    Write-Host "Invalid action: $action" -ForegroundColor Red
    Write-Host "Usage: build or run"
    exit 1
}