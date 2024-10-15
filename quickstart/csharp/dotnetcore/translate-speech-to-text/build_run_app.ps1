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

        & $dotnetTempPath add .\helloworld package Microsoft.CognitiveServices.Speech --interactive
        if ($?) {
            Write-Host "Installation Microsoft.CognitiveServices.Speech package is succeeded." -ForegroundColor Green
        }
        else {
            Write-Host "Installation Microsoft.CognitiveServices.Speech package is failed, exiting..." -ForegroundColor Red
            exit 1
        }

        & $dotnetTempPath build .\helloworld --configuration release
        if ($?) {
            Write-Host "Building is succeeded." -ForegroundColor Green
        }
        else {
            Write-Host "Building is failed, exiting..." -ForegroundColor Red
            exit 1
        }
    }
    else {
        & dotnet add .\helloworld package Microsoft.CognitiveServices.Speech --interactive
        if ($?) {
            Write-Host "Installation Microsoft.CognitiveServices.Speech package is succeeded." -ForegroundColor Green
        }
        else {
            Write-Host "Installation Microsoft.CognitiveServices.Speech package is failed, exiting..." -ForegroundColor Red
            exit 1
        }

        & dotnet build .\helloworld --configuration release
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
    if (Get-Command dotnet -ErrorAction SilentlyContinue) {
        & dotnet run --project .\helloworld\helloworld.csproj --configuration release
    }
    elseif (Get-Command $dotnetTempPath -ErrorAction SilentlyContinue) {
        & $dotnetTempPath run --project .\helloworld\helloworld.csproj --configuration release
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
