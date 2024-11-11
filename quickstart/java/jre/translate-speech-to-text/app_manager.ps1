param (
    [string]$action
)

function Install-OpenJDK {
    # Check if JDK is installed
    if (-not (Get-Command java -ErrorAction SilentlyContinue)) {
        Write-Host "Installing OpenJDK..."

        # Download and install OpenJDK (using Chocolatey if available)
        if (Get-Command choco -ErrorAction SilentlyContinue) {
            choco install openjdk11 -y
        }
        else {
            Write-Host "Chocolatey not found. Please install OpenJDK manually." -BackgroundColor Red
            exit 1
        }

        # Check installation result
        if (Get-Command java -ErrorAction SilentlyContinue) {
            Write-Host "OpenJDK was installed successfully. Version:"
            java -version
        }
        else {
            Write-Host "OpenJDK installation failed." -BackgroundColor Red
            exit 1
        }
    }
}

function Install-Maven {
    # Check if Maven is installed
    if (-not (Get-Command mvn -ErrorAction SilentlyContinue)) {
        Write-Host "Maven is not installed, installing..."

        # Download and install Maven (using Chocolatey if available)
        if (Get-Command choco -ErrorAction SilentlyContinue) {
            choco install maven -y
        }
        else {
            Write-Host "Chocolatey not found. Please install Maven manually."
            exit 1
        }

        if (-not (Get-Command mvn -ErrorAction SilentlyContinue)) {
            Write-Host "Maven installation failed." -BackgroundColor Red
            exit 1
        }
    }
}

if ($action -eq "configure") {
    Install-OpenJDK
    Install-Maven

    # Install dependencies for the Java project
    Write-Host "Installing Java project dependencies..."
    Start-Process -NoNewWindow -Wait -FilePath "mvn" -ArgumentList "clean install"
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Java project dependencies installation failed." -BackgroundColor Red
        exit 1
    }
    Write-Host "Configuration completed successfully."

}
elseif ($action -eq "build") {
    Write-Host "Compiling Java files..."
    Start-Process -NoNewWindow -Wait -FilePath "mvn" -ArgumentList "compile"
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Java file compilation failed." -BackgroundColor Red
        exit 1
    }
    Write-Host "Compilation succeeded."

}
elseif ($action -eq "run") {
    Write-Host "Running Java application..."
    Start-Process -NoNewWindow -Wait -FilePath "mvn" -ArgumentList "exec:java -Dexec.mainClass=speechsdk.quickstart.Main"
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Java application execution failed."
        exit 1
    }
    Write-Host "Java application ran successfully."

}
else {
    Write-Host "Invalid action: $action"
    Write-Host "Usage: {build|run|configure}"
    exit 1
}
