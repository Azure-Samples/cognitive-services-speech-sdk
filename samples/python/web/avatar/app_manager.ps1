param(
    [string]$action
)

function Test-PythonInstalled {
    return Get-Command python -ErrorAction SilentlyContinue
}

function Test-PipInstalled {
    return Get-Command pip -ErrorAction SilentlyContinue
}

if ($action -eq "configure") {
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
    # Define the path to your .env file
    $envFilePath = ".env/.env.dev"

    if (Test-Path $envFilePath) {
        # Read each line of the file and process it
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

            [System.Environment]::SetEnvironmentVariable("SPEECH_KEY", $env:SPEECH_RESOURCE_KEY)
            [System.Environment]::SetEnvironmentVariable("AZURE_OPENAI_API_KEY", $env:SPEECH_RESOURCE_KEY)
            [System.Environment]::SetEnvironmentVariable("SPEECH_REGION", $env:SERVICE_REGION)
            [System.Environment]::SetEnvironmentVariable("AZURE_OPENAI_ENDPOINT", "https://$env:CUSTOM_SUBDOMAIN_NAME.openai.azure.com/")
        }

        Write-Host "Environment variables loaded from $envFilePath"
    }
    else {
        Write-Host "File not found: $envFilePath. You can create one to set environment variables or manually set secrets in environment variables."
    }

    Start-Process "python" -ArgumentList "-m", "flask", "run", "-h", "0.0.0.0", "-p", "5000"

    # Add a small delay to give the server time to start
    Start-Sleep -Seconds 5

    # Open the URL in the default browser
    Start-Process "http://127.0.0.1:5000"

    # Keep the terminal session alive to prevent VS Code from closing the terminal and stopping the server
    Write-Host "Server is running. Press any key to exit." -ForegroundColor Green
    [System.Console]::ReadKey($true) | Out-Null

}
else {
    Write-Host "Invalid action: $action" -ForegroundColor Red
    Write-Host "Usage: -action configure or -action run"
    exit 1
}
