param(
    [string]$action
)

$dotnetPath = "C:\Program Files\dotnet\dotnet.exe"
$dotnetInstallationTempDirectory = "$env:LOCALAPPDATA\dotnet"
$dotnetTempPath = Join-Path $dotnetInstallationTempDirectory "dotnet.exe"

function Install-DotNet8 {
    Write-Host "Installing .NET SDK 8.0..."
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

    $dotnetPath = $dotnetTempPath
    Write-Host ".NET 8.0 installed successfully." -ForegroundColor Green
    Remove-Item -Force dotnet-install.ps1
}

if ($action -eq "configure") {
    if (-not (Get-Command dotnet -ErrorAction SilentlyContinue) -or ([version]$(dotnet --version) -lt [version]"8.0")) {
        Install-DotNet8
    } else {
        Write-Host ".NET 8.0 is already installed." -ForegroundColor Green
    }
}
elseif ($action -eq "build") {
    & $dotnetPath build batchclient/batchclient.csproj
    if (! $?) {
        Write-Host "Building is failed, exiting..." -ForegroundColor Red
        exit 1
    }
}
elseif ($action -eq "run") {
    # Define the path to your .env file
    $envFilePath = ".env/.env.dev"
    if (Test-Path $envFilePath) {
        Get-Content -Path $envFilePath | ForEach-Object {
            # Ignore empty lines and lines that start with `#` (comments)
            if ($_ -and $_ -notmatch '^\s*#') {
                # Split each line into key and value
                $parts = $_ -split '=', 2
                $key = $parts[0].Trim()
                $value = $parts[1].Trim()

                # Set the environment variable
                [System.Environment]::SetEnvironmentVariable($key, $value)
            }
        }

        [System.Environment]::SetEnvironmentVariable("SPEECH_KEY", $env:SPEECH_RESOURCE_KEY)
        [System.Environment]::SetEnvironmentVariable("SPEECH_REGION", $env:SERVICE_REGION)
    }
    else {
        Write-Host "File not found: $envFilePath. You can create one to set environment variables or manually set secrets in environment variables."
    }

    if (Get-Command $dotnetPath -ErrorAction SilentlyContinue) {
        $recordingsBlobUri = Read-Host "Please enter SAS URI pointing to audio files stored in Azure Blob Storage. If input multiple, please separate them with commas."
        if ([string]::IsNullOrWhiteSpace($recordingsBlobUri)) {
            Write-Host "Not enter the Azure Blob SAS URL of the input audio file." -ForegroundColor Red
            exit 1
        }

        $recordingsLocale = Read-Host "Please enter the locale of the input audio file (e.g. en-US, zh-CN, etc.)"
        if ([string]::IsNullOrWhiteSpace($recordingsLocale)) {
            Write-Host "Not enter the locale." -ForegroundColor Red
            exit 1
        }

        & $dotnetPath batchclient/bin/Debug/net8.0/BatchTranscriptions.dll --recordingsBlobUri $recordingsBlobUri --locale  $recordingsLocale
    }
    else {
        Write-Host ".NET SDK is not found. Please first run the script with build action to install .NET 8.0." -ForegroundColor Red
        exit 1
    }
}
else {
    Write-Host "Invalid action: $action" -ForegroundColor Red
    Write-Host "Usage: build, run, or configure"
    exit 1
}
