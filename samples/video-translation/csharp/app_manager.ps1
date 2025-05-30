param(
    [string]$action
)

$dotnetPath = "C:\Program Files\dotnet\dotnet.exe"
$dotnetInstallationTempDirectory = "$env:LOCALAPPDATA\dotnet"
$dotnetTempPath = Join-Path $dotnetInstallationTempDirectory "dotnet.exe"

function Install-DotNet {
    Invoke-WebRequest -Uri https://dot.net/v1/dotnet-install.ps1 -OutFile dotnet-install.ps1
    if (-not $?) {
        Write-Host "Failed to download dotnet-install.ps1, exiting..." -ForegroundColor Red
        exit 1
    }

    & .\dotnet-install.ps1 -InstallDir $dotnetInstallationTempDirectory -Version 7.0.410
    if (-not $?) {
        Write-Host "Failed to install .NET 7.0 SDK, exiting..." -ForegroundColor Red
        exit 1
    }

    Write-Host ".NET 7.0 installed successfully." -ForegroundColor Green
    Remove-Item -Force dotnet-install.ps1

    $dotnetPath = $dotnetTempPath
}

if ($action -eq "build") {
    if (-not (Get-Command dotnet -ErrorAction SilentlyContinue) -or ([version]$(dotnet --version) -lt [version]"7.0")) {
        Write-Host "Installing .NET SDK 7.0..."

        Install-DotNet
    }

    & $dotnetPath build VideoTranslationSample/VideoTranslationSample/VideoTranslationSample.csproj
    if (! $?) {
        Write-Host "Building is failed, exiting..." -ForegroundColor Red
        exit 1
    }
}
elseif ($action -eq "run") {
    $configFilePath = "config.json"
    if (Test-Path $configFilePath) {
        $jsonContent = Get-Content -Path $configFilePath -Raw | ConvertFrom-Json
        $subscriptionKey = $jsonContent.SubscriptionKey
        $subscriptionRegion = $jsonContent.ServiceRegion
    }
    else {
        Write-Host "The config.json file is not found." -ForegroundColor Red
        exit 1
    }

    if ($subscriptionRegion -ne "eastus") {
        Write-Host "The sample only supports the eastus region, please reconfigure the Azure Speech Resources." -ForegroundColor Red
        exit 1
    }

    if (Get-Command $dotnetPath -ErrorAction SilentlyContinue) {
        $videoFileAzureBlobUrl = Read-Host "Please enter the Azure Blob URL (for example, Azure blob SAS token URL) of the input video file for translation"
        if ([string]::IsNullOrWhiteSpace($videoFileAzureBlobUrl)) {
            Write-Host "Not enter the Azure Blob URL of the input video file." -ForegroundColor Red
            exit 1
        }

        $sourceLocale = Read-Host "Please enter the source locale of the video (e.g. en-US, zh-CN, etc.)"
        if ([string]::IsNullOrWhiteSpace($sourceLocale)) {
            Write-Host "Not enter the source locale." -ForegroundColor Red
            exit 1
        }

        $targetLocale = Read-Host "Please enter the target locale of the video (e.g. en-US, zh-CN, etc. Dafalut is en-US)"
        if ([string]::IsNullOrWhiteSpace($targetLocale)) {
            $targetLocale = "en-US"
        }

        $voiceKind = Read-Host "Please enter the TTS synthesis voice kind: PlatformVoice or PersonalVoice (Default is PlatformVoice)"
        if ([string]::IsNullOrWhiteSpace($voiceKind)) {
            $voiceKind = "PlatformVoice"
        }

        $validVoices = @("PlatformVoice", "PersonalVoice")
        if ($voiceKind -notin $validVoices) {
            Write-Host "Wrong voice kind $voiceKind entered." -ForegroundColor Red
            exit 1
        }

        $translationId = Read-Host "Please enter the translation resource ID (example: PersonalRecording_en-US_2024_110501)"
        if ([string]::IsNullOrWhiteSpace($translationId)) {
            Write-Host "Not enter the translation resource ID." -ForegroundColor Red
            exit 1
        }

        $iterationId = Read-Host "Please enter the iteration resource ID (example: Iteration-2024110501)"
        if ([string]::IsNullOrWhiteSpace($iterationId)) {
            Write-Host "Not enter the iteration resource ID." -ForegroundColor Red
            exit 1
        }

        $projectPath = "VideoTranslationSample/VideoTranslationSample/bin/Debug/net7.0/Microsoft.SpeechServices.VideoTranslation.ApiSampleCode.PublicPreview.dll"
        $commandLineArgs = @(
            "-mode", "CreateTranslationAndIterationAndWaitUntilTerminated",
            "-apiVersion", "2024-05-20-preview",
            "-subscriptionKey", $subscriptionKey,
            "-region", $subscriptionRegion,
            "-sourceLocale", $sourceLocale,
            "-targetLocale", $targetLocale,
            "-voiceKind", $voiceKind,
            "-videoFileAzureBlobUrl", $videoFileAzureBlobUrl,
            "-translationId", $translationId,
            "-iterationId", $iterationId
        )
        Write-Host "Running command: $dotnetPath $projectPath $($commandLineArgs -join ' ')" -ForegroundColor Green
        & $dotnetPath $projectPath $commandLineArgs
    }
    else {
        Write-Host ".NET SDK is not found. Please first run the script with build action to install .NET 7.0." -ForegroundColor Red
        exit 1
    }
}
else {
    Write-Host "Invalid action: $action" -ForegroundColor Red
    Write-Host "Usage: build or run"
    exit 1
}