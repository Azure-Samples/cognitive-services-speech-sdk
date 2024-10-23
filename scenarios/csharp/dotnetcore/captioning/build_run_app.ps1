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


if ($action -eq "build") {
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
    $envFilePath = ".env/.env.dev"
    if (Test-Path $envFilePath) {
        Get-Content $envFilePath | ForEach-Object {
            if ($_ -and $_ -notmatch '^\s*#') {
                $pair = $_ -split '='
                $key = $pair[0].Trim()
                $value = $pair[1].Trim()
    
                if ($key -eq "SPEECH_RESOURCE_KEY") {
                    [System.Environment]::SetEnvironmentVariable("SPEECH_KEY", $value)
                }
                elseif ($key -eq "SERVICE_REGION") {
                    [System.Environment]::SetEnvironmentVariable("SPEECH_REGION", $value)
                }
            }
        }
        Write-Host "Environment variables loaded from $envFilePath"
    }
    else {
        Write-Host "File not found: $envFilePath"
    }

    if (Get-Command dotnet -ErrorAction SilentlyContinue) {
        & dotnet run --project .\captioning\captioning.csproj --configuration release --input Sample.mp4 --format any --output caption.output.txt --srt --realTime --threshold 5 --delay 0 --profanity mask --phrases "Contoso;Jessie;Rehaan"
    }
    elseif (Get-Command $dotnetTempPath -ErrorAction SilentlyContinue) {
        & $dotnetTempPath run --project .\captioning\captioning.csproj --configuration release --input Sample.mp4 --format any --output caption.output.txt --srt --realTime --threshold 5 --delay 0 --profanity mask --phrases "Contoso;Jessie;Rehaan"
    }
    else {
        Write-Host ".NET SDK is not found. Please first run the script with build action to install .NET 6.0." -ForegroundColor Red
        exit 1
    }
}
else {
    Write-Host "Invalid action: $action" -ForegroundColor Red
    Write-Host "Usage: build or run"
    exit 1
}