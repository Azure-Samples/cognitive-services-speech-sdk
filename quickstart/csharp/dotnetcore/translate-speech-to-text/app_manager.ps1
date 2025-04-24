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

    & .\dotnet-install.ps1 -InstallDir $dotnetInstallationTempDirectory -Version 8.0.408
    if (-not $?) {
        Write-Host "Failed to install .NET SDK, exiting..." -ForegroundColor Red
        exit 1
    }

    $dotnetPath = $dotnetTempPath
    Write-Host ".NET 6 installed successfully." -ForegroundColor Green
    Remove-Item -Force dotnet-install.ps1
}

if ($action -eq "configure") {
    if (-not (Get-Command dotnet -ErrorAction SilentlyContinue) -or ([version]$(dotnet --version) -lt [version]"8.0")) {
        Install-DotNet8
    } else {
        Write-Host ".NET 8 is already installed." -ForegroundColor Green
    }
}
elseif ($action -eq "build") {
    & $dotnetPath build helloworld/helloworld.csproj
    if (! $?) {
        Write-Host "Building is failed, exiting..." -ForegroundColor Red
        exit 1
    }
}
elseif ($action -eq "run") {
    if (Get-Command $dotnetPath -ErrorAction SilentlyContinue) {
        & $dotnetPath helloworld/bin/Debug/net8.0/helloworld.dll
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
