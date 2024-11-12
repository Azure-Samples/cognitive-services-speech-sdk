param (
    [string]$action
)

$downloadPath = "$env:TEMP"
$javaInstallPath = Join-Path $env:LOCALAPPDATA "Java"
$mavenInstallPath = Join-Path $env:LOCALAPPDATA "Maven"

if ($env:PROCESSOR_ARCHITECTURE -ne "AMD64") {
    Write-Host "Your Windows is not 64-bit architecture. Please use a computer with a 64-bit architecture running Windows 10 or later." -ForegroundColor Red
    exit 1
}

# Registry path: Visual C++ Redistributable installation information
$regKeyPath = "HKLM:\SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\X64"
# Check if it is installed
$installed = Get-ItemProperty -Path $regKeyPath | Select-Object "Installed"
if (-not $installed) {
    Write-Host "The Visual C++ 2015-2022 Redistributable is not installed. Downloading and installing..."

    # Download the installer
    $installerPath = "$env:TEMP\vc_redist_x64_installer.exe"
    Invoke-WebRequest -Uri "https://aka.ms/vs/17/release/vc_redist.x64.exe" -OutFile $installerPath
    if (-not $?) {
        Write-Host "Failed to download vc_redist.x64, exiting..." -ForegroundColor Red
        exit 1
    }

    # Install Redistributable (Use silent installation)
    Start-Process -FilePath $installerPath -ArgumentList "/quiet", "/norestart" -Wait
    if (-not $?) {
        Write-Host "Failed to install Visual C++ 2015-2022 Redistributable, exiting..." -ForegroundColor Red
        exit 1
    }

    Write-Host "The Visual C++ 2015-2022 Redistributable installation completed."

    Remove-Item -Force $installerPath
}

# Function to download and extract a zip file
function Get-And-Extract ($url, $destinationPath, $zipPrefix) {
    $tempZip = Join-Path $downloadPath "$zipPrefix.zip"

    Invoke-WebRequest -Uri $url -OutFile $tempZip
    if (-not $?) {
        Write-Host "Failed to download $zipPrefix, exiting..." -ForegroundColor Red
        exit 1
    }

    if (Test-Path $destinationPath) {
        Remove-Item -Recurse -Force $destinationPath
    }

    Expand-Archive -Path $tempZip -DestinationPath $destinationPath
    if (-not $?) {
        Write-Host "Failed to install $zipPrefix, exiting..." -ForegroundColor Red
        exit 1
    }

    Remove-Item -Force $tempZip
}

function Install-OpenJDK {
    Get-And-Extract -url "https://aka.ms/download-jdk/microsoft-jdk-11.0.25-windows-x64.zip" -destinationPath $javaInstallPath -zipPrefix "microsoft-openjdk-11"
    $javaExecutable = Get-ChildItem -Path $javaInstallPath -Recurse -Filter "java.exe" -ErrorAction SilentlyContinue
    $javaHomePath = $javaExecutable.FullName -replace '\\bin\\java\.exe$', ''

    $currentJavaHome = [Environment]::GetEnvironmentVariable("JAVA_HOME", [EnvironmentVariableTarget]::User)
    if ($currentJavaHome -ne $javaHomePath) {
        [Environment]::SetEnvironmentVariable("JAVA_HOME", $javaHomePath, [EnvironmentVariableTarget]::User)
        Write-Host "JAVA_HOME has been set as: $javaHomePath"
    }

    $currentUserPath = [System.Environment]::GetEnvironmentVariable("PATH", [System.EnvironmentVariableTarget]::User)
    $javaJDKPath = "$javaHomePath\bin"
    if ($currentUserPath -notlike "*$javaJDKPath*") {
        $newUserPathValue = "$currentUserPath;$javaJDKPath"
        [System.Environment]::SetEnvironmentVariable("PATH", $newUserPathValue, [System.EnvironmentVariableTarget]::User)
    }

    Write-Output "Java JDK 11 installed to $javaInstallPath."
}

function Find-OpenJDK {
    # Check if Java is installed and if the version is at least 11
    if (-not (Get-Command java -ErrorAction SilentlyContinue)) {
        Write-Output "Installing Java JDK 11..."
        Install-OpenJDK
    }
    else {
        $javaVersion = & java -version 2>&1 | Select-String -Pattern 'version "(\d+)\.(\d+)\.(\d+)"' | ForEach-Object { $_.Matches.Groups[1].Value }
        if ($javaVersion -lt 11) {
            Write-Host "The installed Java version is lower than 11, installing Microsoft Build of OpenJDK 11..."
            Install-OpenJDK
        }
    }
}

function Install-Maven {
    # Check if Maven is installed
    if (-not (Get-Command mvn -ErrorAction SilentlyContinue)) {
        Write-Output "Installing Maven..."
        Get-And-Extract -url "https://archive.apache.org/dist/maven/maven-3/3.9.9/binaries/apache-maven-3.9.9-bin.zip" -destinationPath $mavenInstallPath -zipPrefix "apache-maven"

        $currentUserPath = [System.Environment]::GetEnvironmentVariable("PATH", [System.EnvironmentVariableTarget]::User)
        $mvnCmd = Join-Path $mavenInstallPath "apache-maven-3.9.9\bin"
        if ($currentUserPath -notlike "*$mvnCmd*") {
            $newUserPathValue = "$currentUserPath;$mvnCmd"
            [System.Environment]::SetEnvironmentVariable("PATH", $newUserPathValue, [System.EnvironmentVariableTarget]::User)
        }

        Write-Output "Maven installed to $mavenInstallPath."
    }
}

if ($action -eq "configure") {
    Find-OpenJDK
    Install-Maven

    # Install dependencies for the Java project in pom.xml
    Write-Output "Resolving Maven dependencies..."
    & mvn dependency:resolve
    if (-not $?) {
        Write-Host "Java project dependencies installation failed." -BackgroundColor Red
        exit 1
    }
}
elseif ($action -eq "build") {
    Write-Host "Compiling Java files..."
    & mvn compile
    if (-not $?) {
        Write-Host "Java file compilation failed." -BackgroundColor Red
        exit 1
    }
    Write-Host "Compilation succeeded."
}
elseif ($action -eq "run") {
    Write-Host "Running Java application..."
    & mvn exec:java -Dexec.mainClass="speechsdk.quickstart.Main"
}
else {
    Write-Host "Invalid action: $action"
    Write-Host "Usage: {build|run|configure}"
    exit 1
}
