# Set project directory using relative path
$ProjectDir = ".\captioning"

# Change to project directory
Set-Location $ProjectDir

# Function to install .NET 6.0 SDK
function Install-DotnetSdk {
    Write-Host ".NET 6.0 SDK is not installed. Installing..."

    # Download and install .NET 6.0 SDK
    $url = "https://dotnet.microsoft.com/download/dotnet/6.0"
    Start-Process -FilePath "msiexec.exe" -ArgumentList "/i $url /quiet /norestart" -Wait

    # Check version again
    $dotnetVersion = & dotnet --version 2>$null
    if ($dotnetVersion -ne "6.0") {
        Write-Host "Failed to install .NET 6.0 SDK. Please install it manually."
        exit 1
    }
}

# Check if action variable is set
if (-not $action) {
    Write-Host "Action is not specified. Use 'build' or 'run'." -ForegroundColor Red
    exit 1
}

if ($action -eq "build") {
    # Check .NET SDK version
    $dotnetVersion = & dotnet --version 2>$null

    # Check if .NET 6.0 SDK is installed
    if (-not $dotnetVersion -or $dotnetVersion -ne "6.0") {
        Install-DotnetSdk
    }

    # Install NuGet package Microsoft.CognitiveServices.Speech
    $packageName = "Microsoft.CognitiveServices.Speech"
    dotnet add package $packageName

    # Check if package installation was successful
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Failed to install NuGet package $packageName. Please check the error."
        exit $LASTEXITCODE
    }

    # Run dotnet build command
    dotnet build

    # Check if compilation was successful
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Compilation failed. Please check the error."
        exit $LASTEXITCODE
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

    # Run the generated executable
    $exePath = Join-Path $ProjectDir "bin\Debug\net6.0\captioning.exe"

    # Check if executable file exists
    if (Test-Path $exePath) {
        Write-Host "Running the generated executable..."
        & $exePath
    }
    else {
        Write-Host "Executable not found. Please check the project settings."
    }
}
else {
    Write-Host "Invalid action: $action" -ForegroundColor Red
    Write-Host "Usage: build or run"
    exit 1
}
