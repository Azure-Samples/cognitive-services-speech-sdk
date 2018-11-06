#Requires -RunAsAdministrator
[CmdletBinding()]
Param(
  [Parameter(Mandatory=$true)]
  [ValidateScript({ Test-Path $_})]
  [string]
  $Solution,

  [string]
  [ValidateSet('x64', 'x86')]
  $Platform = 'x64',

  [string]
  [ValidateSet('Debug', 'Release')]
  $Configuration = 'Release',

  [Parameter(Mandatory=$true)]
  [string]
  $CertPath,

  # This is only meant for test certificates!
  [string]
  $CertPassword = "",

  # Where to place the output
  [Parameter(Mandatory=$true)]
  [string]
  [ValidateScript({ -not (Test-Path $_)})]
  $Path
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

# Get common functions
$scriptDir = Split-Path -Path $MyInvocation.MyCommand.Definition -Parent
. (Join-Path $scriptDir functions.ps1)

# Define and check for required commands
$vsInstallationPath = Get-VisualStudio2017InstallationPath
$windowsSdkDir = Get-VisualStudioEnvironmentSetting $vsInstallationPath WindowsSdkDir
$msbuild = "$vsInstallationPath\MSBuild\15.0\bin\MSBuild.exe"
$signtool = "$windowsSdkDir\bin\x64\signtool.exe"
$appCert = "$windowsSdkDir\App Certification Kit\appcert.exe"
$null = Get-Command $msbuild, $signtool, $appcert

# Normalize paths (note: for non-existing cannot use Resolve-Path)
$CertPath = Resolve-Path $CertPath
$Path = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($Path)
if (-not (Test-Path (Split-Path $Path) -PathType Container)) {
  throw "Parent directory of $Path does not exist"
}

& $msbuild /m /p:Configuration=$Configuration /p:Platform=$Platform `
  /p:UapAppxPackageBuildMode=StoreUpload /p:AppxBundlePlatforms=$Platform /p:AppxBundle=Always `
  $Solution
if ($LASTEXITCODE -ne 0) {
  throw "msbuild failed"
}

# Find the .appx packages, sort by length (largest first)
$appxPath = Get-ChildItem -Path (Split-Path $Solution) -Recurse -File -Include *_$Platform.appx | Sort-Object Length -Descending
if ($CertPassword) {
  $passwordSecureString = ConvertTo-SecureString -String $CertPassword -Force -AsPlainText
} else {
  # Special case, also to make sure signtool invocation below from Powershell works.
  $CertPassword = '""'
  $passwordSecureString = New-Object System.Security.SecureString
}
& $signtool sign /fd SHA256 /a /f $CertPath /p $CertPassword $appxPath[0]

# Run the WACK utils. Default to non-passing result

$overallResultPass = $false
$importedCert = $null

try {
  $importedCert = Import-PfxCertificate -Verbose -FilePath $CertPath -CertStoreLocation Cert:\LocalMachine\Root -Password $passwordSecureString

  # TODO we haven't run into this, but the appcert help says: "Please ensure
  # that you have run your application(s) at least once and dismissed license
  # agreements and first-time prompts to avoid unnecessary failures while
  # testing." ?

  & $appcert reset
  if ($LASTEXITCODE -ne 0) {
    throw "appcert reset failed"
  }

  & $appcert test -appxpackagepath $appxPath[0] -reportoutputpath $Path
  if ($LASTEXITCODE -eq 1) {
    Write-Verbose "appcert test executed successfully but needs report finalization."
    & $appcert finalizereport -reportfilepath $Path
    if ($LASTEXITCODE -ne 1) {
      throw "appcert finalizereport failed"
    }
  } elseif ($LASTEXITCODE -ne 0) {
    throw "appcert test failed"
  }

  $report = [xml](Get-Content $Path)

  $overallResultPass = $report.REPORT.OVERALL_RESULT -eq 'PASS'
} finally {
  if ($importedCert) {
    $importedCert | Remove-Item -Verbose
  }
}

if (-not $overallResultPass) {
  throw "WACK check failed"
} else {
  Write-Verbose "WACK check passed, generated $Path"
}
