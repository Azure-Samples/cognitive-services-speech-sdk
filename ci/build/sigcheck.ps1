<#
.SYNOPSIS
Check Speech SDK files for valid signatures

.DESCRIPTION
Files shipped by the Speech SDK need to be signed, check the signatures with this script.

.PARAMETER RootDirectory
This is the root directory for the files to check (artifact-directory)
#>

[CmdletBinding()]
Param(
   [Parameter(Mandatory=$true)]
   [ValidateScript({ Test-Path $_ })]
   [string] $RootDirectory)

function CheckDirectory([string] $directory)
{
    if (-not (Test-Path -LiteralPath $directory -PathType Container)) {
        throw "Directory ($Directory) doesn't exist. "
    }
}

function CheckNuget
{
    $filename = "nuget.exe"
    if ((Get-Command "$filename" -ErrorAction SilentlyContinue) -eq $null) {
        throw "Unable to find $filename in PATH"
    }
    $script:nugetExe = "NuGet.exe"
}

function CheckJarsigner
{
    $filename = "jarsigner.exe"
    if ((Get-Command "$filename" -ErrorAction SilentlyContinue) -eq $null) {
        $dir = Join-Path $env:JAVA_HOME bin
        $filename = Join-Path $dir $filename

        if ((Get-Command "$filename" -ErrorAction SilentlyContinue) -eq $null) {
            throw "Unable to find jarsigner in PATH or env:JAVA_HOME\\bin"
        }
    }
    $script:jarsignerExe = $filename
}

function CheckSigntool
{
    $filename = "signtool.exe"
    if ((Get-Command "$filename" -ErrorAction SilentlyContinue) -eq $null) {
        $vswhere = "c:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
        $installationPath = & $vswhere -prerelease -latest -property installationPath   # requires Windows SDK
        $WindowsSdkVerBinPath = $null

        if ($installationPath -and (test-path "$installationPath\Common7\Tools\vsdevcmd.bat")) {
            & "${env:COMSPEC}" /s /c "`"$installationPath\Common7\Tools\vsdevcmd.bat`" -no_logo && set" | foreach-object {
                $name, $value = $_ -split '=', 2
                if ($name -eq 'WindowsSdkVerBinPath') {
                    $WindowsSdkVerBinPath = $value
                }
            }
        }
        if (-not $WindowsSdkVerBinPath) {
            throw "Could not find Windows SDK for signtool"
        }
        $WindowsSdkVerBinPath = Join-Path $WindowsSdkVerBinPath "x86"
        $filename = Join-Path $WindowsSdkVerBinPath $filename
        if ((Get-Command "$filename" -ErrorAction SilentlyContinue) -eq $null) {
            throw "Couldn't find SIGNTOOL inside the Windows SDK"
        }
    }
    $script:signtoolExe = $filename
 }

function RunJarsigner(
    [parameter(Mandatory=$true)][string] $filename)
{
    Write-Host Checking $filename
    & "$script:jarsignerExe" "-verify" "-strict" "$filename"  | Out-Null
    if ($LASTEXITCODE -ne 0) {
        $script:errorFound = $true
        Write-Host "`nFAILURE: jarsigner failure for file $filename.`n"
    }
}

function RunNuget(
    [parameter(Mandatory=$true)][string] $filename)
{
    Write-Host Checking $filename
    & "$script:nugetExe" "verify" "-Signatures" "$filename" "-CertificateFingerprint" "3f9001ea83c560d712c24cf213c3d312cb3bff51ee89435d3430bd06b5d0eece" | Out-Null
    if ($LASTEXITCODE -ne 0) {
        $script:errorFound = $true
        Write-Host "`nFAILURE: nuget failure for file $filename.`n"
    }
}

function RunSigntool(
    [string][parameter(Mandatory=$true)] $filename)
{
    Write-Host Checking $filename
    & "$script:signtoolExe" "verify" "/pa" "$filename" | Out-Null
    if ($LASTEXITCODE -ne 0) {
        $script:errorFound = $true
        Write-Host "`nFAILURE: signtool failure for file $filename.`n"
    }
}

function ExpandIntoDirectory (
    [string][parameter(Mandatory=$true)] $workDirectory,
    [string][parameter(Mandatory=$true)] $workZip,
    [string][parameter(Mandatory=$true)] $filename)
{
    Remove-Item $workDirectory -Recurse -Force -ErrorAction SilentlyContinue | Out-Null
    Remove-Item $workZip -Force -ErrorAction SilentlyContinue | Out-Null
    New-Item $workDirectory -Type Directory -Force -ErrorAction SilentlyContinue | Out-Null
    Copy-Item $filename $workZip -Force -ErrorAction Stop | Out-Null

    Microsoft.PowerShell.Archive\Expand-Archive $workZip -DestinationPath $workDirectory -Force | Out-Null
}

function Check_ar(
    [string][parameter(Mandatory=$true)] $directory,
    [string][parameter(Mandatory=$true)] $filter)
{
    CheckDirectory -directory $directory

    $files = Get-ChildItem $directory -file -recurse -filter $filter
    foreach($entry in $files) {
        $filename = $entry.Fullname

        $whiteList | foreach {
            if ($filename -match $_) {
                Write-Host "Skipping  $filename"
                continue
            }
        }

        RunJarsigner -filename $filename

        $workDirectory = Join-Path $directory "tmpdir"
        $workZip = Join-Path $directory "temp.zip"

        ExpandIntoDirectory -workDirectory $workDirectory -workZip $workZip -filename $filename
        # this file could contain more JAR files or DLLs
        Check_ar -directory $workDirectory -filter "*.jar"
        CheckWindowsDlls -directory $workDirectory

        Remove-Item $workDirectory -Recurse -Force -ErrorAction SilentlyContinue | Out-Null
        Remove-Item $workZip -Force -ErrorAction SilentlyContinue | Out-Null
    }
}

function CheckWindowsNuget(
    [string][parameter(Mandatory=$true)] $directory)
{
    CheckDirectory -directory $directory

    $files = Get-ChildItem $directory -file -recurse -filter *.nupkg
    foreach($entry in $files) {
        $filename = $entry.Fullname

        $whiteList | foreach {
            if ($filename -match $_) {
                Write-Host "Skipping  $filename"
                continue
            }
        }

        RunNuget -filename $filename

        $workDirectory = Join-Path $directory "tmpdir"
        $workZip = Join-Path $directory "temp.zip"

        ExpandIntoDirectory -workDirectory $workDirectory -workZip $workZip -filename $filename

        CheckWindowsDlls -directory $workDirectory

        Remove-Item $workDirectory -Recurse -Force -ErrorAction SilentlyContinue | Out-Null
        Remove-Item $workZip -Force -ErrorAction SilentlyContinue | Out-Null
    }
}

function CheckWindowsWheel(
    [string][parameter(Mandatory=$true)] $directory)
{
    CheckDirectory -directory $directory

    $files = Get-ChildItem $directory -file -recurse -filter *-win_*.whl
    foreach($entry in $files) {
        $filename = $entry.Fullname

        $whiteList | foreach {
            if ($filename -match $_) {
                Write-Host "Skipping  $filename"
                continue
            }
        }

        $workDirectory = Join-Path $directory "tmpdir"
        $workZip = Join-Path $directory "temp.zip"

        ExpandIntoDirectory -workDirectory $workDirectory -workZip $workZip -filename $filename

        CheckWindowsDlls -directory $workDirectory

        Remove-Item $workDirectory -Recurse -Force -ErrorAction SilentlyContinue | Out-Null
        Remove-Item $workZip -Force -ErrorAction SilentlyContinue | Out-Null
    }
}

function CheckWindowsDlls(
    [string][parameter(Mandatory=$true)] $directory)
{
    CheckDirectory -directory $directory

    $files = Get-ChildItem $directory -Recurse -File -Include *.dll, *.pyd
    foreach($entry in $files) {
        $filename = $entry.FullName

        $whiteList | foreach {
            if ($filename -match $_) {
                Write-Host "Skipping  $filename"
                continue
            }
        }
        RunSigntool $filename
    }
}

Function main
{
    try {
        Write-Host Checking executables
        CheckNuget
        CheckSigntool
        CheckJarsigner

        $directory = Join-Path $RootDirectory "Windows"

        Write-Host *** Checking NUPKG files in $directory ***
        CheckWindowsNuget -directory $directory

        $directory = Join-Path $RootDirectory "PythonBuild"

        Write-Host *** Checking WHL files in $directory ***
        CheckWindowsWheel -directory $directory

        $directory = Join-Path $RootDirectory "Android"

        Write-Host *** Checking AAR files in $directory ***
        Check_ar -directory $directory -filter "*.aar"
        Write-Host

        $directory = Join-Path $RootDirectory "JavaJrePackage"

        Write-Host *** Checking JAR files in $directory ***
        Check_ar -directory $directory -filter "*.jar"
        Write-Host
    }
    catch {
        Write-Host "Exception caught - function main / failure"
        Write-Host ($Error[0]).Exception
        exit 1
    }
}

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$script:errorFound = $false
$whiteList = @("Android\\client-sdk-\d+\.\d+\.\d+-.+debug.aar",
               "runtimes\\linux-x64\\native\\libMicrosoft.CognitiveServices.Speech.core.so")
main

if ($script:errorFound) {
    Write-Host
    Write-Host Errors reported during check ...
    Write-Host
    exit 1
}

Write-Host
Write-Host No error to report ...
Write-Host

exit 0
