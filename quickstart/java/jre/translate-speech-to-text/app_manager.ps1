param (
    [string]$action
)

$downloadPath = "$env:TEMP"
$javaCmd = "java"
$mvnCmd = "mvn"
$javaInstallPath = Join-Path $env:LOCALAPPDATA "Java\jdk-11"
$mavenInstallPath = Join-Path $env:LOCALAPPDATA "Maven"

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
    # Check if Java is installed and if the version is at least 11
    $javaVersion = & java -version 2>&1 | Select-String -Pattern 'version "(?<version>\d+\.\d+).*"' | ForEach-Object { $_.Matches[0].Groups["version"].Value }
    if (-not $javaVersion -or [version]$javaVersion -lt [version]"11.0") {
        Write-Output "Installing Java JDK 11..."
        Get-And-Extract -url "https://download.oracle.com/java/11/latest/jdk-11_windows-x64_bin.zip" -destinationPath $javaInstallPath -zipName "jdk-11"

        $javaCmd = Join-Path $javaInstallPath "bin\java.exe"
        # $env:JAVA_HOME = $javaInstallPath
        # $env:Path += ";$javaInstallPath\bin"
        Write-Output "Java JDK 11 installed to $javaInstallPath."
    }
}

function Install-Maven {
    # Check if Maven is installed
    $mavenVersion = & mvn -v 2>&1 | Select-String -Pattern 'Apache Maven (?<version>\d+\.\d+\.\d+)' | ForEach-Object { $_.Matches[0].Groups["version"].Value }
    if (-not $mavenVersion) {
        Write-Output "Installing Maven..."
        Get-And-Extract -url "https://archive.apache.org/dist/maven/maven-3/3.8.8/binaries/apache-maven-3.8.8-bin.zip" -destinationPath $mavenInstallPath -zipName "apache-maven"

        $mvnCmd = Join-Path $mavenInstallPath "apache-maven-3.8.8\bin\mvn.cmd"

        Write-Output "Maven installed to $mavenInstallPath."
    }
}

if ($action -eq "configure") {
    Install-OpenJDK
    Install-Maven

    # Install dependencies for the Java project in pom.xml
    Write-Output "Resolving Maven dependencies..."
    & $mvnCmd dependency:resolve
    if (-not $?) {
        Write-Host "Java project dependencies installation failed." -BackgroundColor Red
        exit 1
    }
}
elseif ($action -eq "build") {
    Write-Host "Compiling Java files..."
    & $mvnCmd compile
    if (-not $?) {
        Write-Host "Java file compilation failed." -BackgroundColor Red
        exit 1
    }
    Write-Host "Compilation succeeded."
}
elseif ($action -eq "run") {
    Write-Host "Running Java application..."
    & $mvnCmd exec:java -Dexec.mainClass="speechsdk.quickstart.Main"
}
else {
    Write-Host "Invalid action: $action"
    Write-Host "Usage: {build|run|configure}"
    exit 1
}
