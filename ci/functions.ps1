# This file sourced from other Powershell scripts.

function Get-VisualStudio2017InstallationPath
{
  [CmdletBinding()]
  Param()

  $vswhere = "c:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
  if (Test-Path -PathType Leaf $vswhere) {
    $installationPath = & $vswhere -version '[15.0,16.0)' -latest -property installationPath
    if ($LASTEXITCODE -ne 0) {
      throw "Running vswhere failed"
    }
    $installationPath
  } else {
    throw "Cannot find vswhere"
  }
}

function Get-VisualStudioEnvironmentSetting {
  [CmdletBinding()]
  Param(
    [Parameter(Mandatory=$true)]
    [ValidateScript({ Test-Path $_})]
    [string]
    $InstallationPath,

    [Parameter(Mandatory=$true)]
    [string]
    $Setting
  )

  if ($InstallationPath -and (Test-Path "$InstallationPath\Common7\Tools\vsdevcmd.bat")) {
    $value = & "${env:COMSPEC}" /s /c "`"$InstallationPath\Common7\Tools\vsdevcmd.bat`" -no_logo && set" | ForEach-Object {
      $name, $value = $_ -split '=', 2
        if ($name -eq $Setting) {
          $value
        }
    }
    if ($value) {
      $value
    } else {
      throw "Cannot find setting $Setting"
    }
  } else {
    throw "Cannot find vsdevcmd.bat"
  }
}
