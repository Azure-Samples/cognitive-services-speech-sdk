[CmdletBinding()]
Param(
  # Cooked output data.
  [string]
  [Parameter(Mandatory = $true)]
  [ValidateScript({ Test-Path -PathType Container -LiteralPath $_})]
  $CookedDir,

  # Date and time for this instant (allow to override for test purposes)
  [datetime]
  $Now = [datetime]::UtcNow,

  [switch]
  $DownloadCooked,

  [switch]
  $UploadCooked,

  [string]
  $BlobStorageKey,

  [string]
  $BlobCookedUrl = 'https://csspeechstorage.blob.core.windows.net/usagedata/nuget',

  [switch]
  $Force
)

$azcopy = "c:\Program Files (x86)\Microsoft SDKs\Azure\AzCopy\AzCopy.exe"
$needBlobOperation = $DownloadCooked -or $UploadCooked
if ($needBlobOperation -and -not $BlobStorageKey) {
  throw "-BlobStorageKey required with download/upload"
}

if ($needBlobOperation -and -not (Test-Path $azcopy)) {
  throw "azcopy required for up/down-load but not found here: $azcopy"
}

if ($UploadCooked -and -not ($DownloadCooked -or $Force)) {
  throw "-DownloadCooked or -Force required for -UploadCooked"
}

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

# Normalize
$CookedDir = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($CookedDir)

if ($DownloadCooked) {
  Write-Verbose "Downloading cooked data from $BlobCookedUrl"
  & $azcopy /y /SourceKey:$BlobStorageKey /Source:$BlobCookedUrl /s /Dest:$CookedDir /mt /xo
  if ($LASTEXITCODE -ne 0) {
    throw "azcopy operation for downloading cooked logs failed"
  }
}

function writeData($data, [string]$path) {

  $newData = "$path.new"

  $oldData = $false
  $dataChanged = $true

  if (Test-Path -LiteralPath $path -PathType Container) {
    throw "$path exists as a directory"
  } elseif (Test-Path -LiteralPath $path -PathType Leaf) {
    $oldData = $true
  } else {
    Write-Verbose "$path does not exist yet"
    $dir = Split-Path -LiteralPath $Path
    if ($dir) {
      if (Test-Path -LiteralPath $dir -PathType Leaf) {
        throw "$dir exists as a file"
      } elseif (Test-Path -LiteralPath $dir -PathType Container) {
        # Do nothing.
      } else {
        Write-Verbose "Creating directory $dir"
        $null = New-Item -ItemType Directory -Path $dir
      }
    }
  }

  # Write as UTF8 w/o BOM
  $output = $data | ConvertTo-Csv -NoTypeInformation
  $Utf8NoBomEncoding = New-Object System.Text.UTF8Encoding $False
  [System.IO.File]::WriteAllLines($newData, $output, $Utf8NoBomEncoding)

  if ($oldData) {
    # If the new data is the same as the old, we will not update the original
    # data (so sync will not pick it up).
    $direction = [math]::Sign((Get-Item -LiteralPath $newData).Length -
      (Get-Item -LiteralPath $path).Length)
    if ($direction -eq 0) {
      # If hash also matches reset $dataChanged
      if ((Get-FileHash -Algorithm SHA256 -LiteralPath $newData).Hash -eq
        (Get-FileHash -Algorithm SHA256 -LiteralPath $path).Hash) {
        $dataChanged = $false
      }
    }
  }

  if ($dataChanged) {
    Write-Verbose "Updating $path"
    Move-Item -LiteralPath $newData -Destination $path -Force
  } else {
    Write-Verbose "Keeping $path"
    Remove-Item -Force -LiteralPath $newData -ErrorAction SilentlyContinue
  }
}

# Capture today's data
$id = 'Microsoft.CognitiveServices.Speech'
$url = 'https://api-v2v3search-0.nuget.org/query?q=microsoft.cognitiveservices.speech&prerelease=true'
$result = Invoke-RestMethod $url

# Potentially multiple packages are above. Filter to just ours.

$pkgResult = @($result.data | Where-Object { $_.id -eq $id })

if ($pkgResult.Length -ne 1) {
  throw "Package info for $id is missing from REST method result: $($result | ConvertTo-Json)"
}

# Update today's record
$today = $Now.Date
$outputDay = "{0}/bydate/{1:yyyy/MM/yyyy-MM-dd}.csv" -f $CookedDir, $today
$nowIso8601 = $Now.ToString("yyyy-MM-ddTHH:mm:ssZ")
$output = $pkgResult.versions |
  Select-Object version, downloads, @{Name = 'timestamp'; Expression = { $nowIso8601 } }
$output += [pscustomobject]@{version = 'ALL'; downloads = $pkgResult.totalDownloads; timestamp = $nowIso8601}
writeData $output $outputDay $false

# Produce overall

$allCooked = @(Get-ChildItem -LiteralPath "$CookedDir/bydate" -Filter *.csv -File -Recurse)
$all = @(Import-Csv -Encoding UTF8 -Path $allCooked.FullName | Where-Object version -eq 'ALL')

$computeDayColumn = @{
  Name = 'Day';
  Expression = {
    ([datetime]($_.timestamp)).Date
  }
}

$overallByDate = $all |
  Select-Object -Property $computeDayColumn, downloads, timestamp |
  Group-Object Day, downloads |
  ForEach-Object {
    $_.Group | Sort-Object timestamp | Select-Object -Last 1
  } |
  Sort-Object Day |
  Select-Object downloads, timestamp

$lastCount = $null

$nugetByDay = $overallByDate | ForEach-Object {
  $delta = if ($lastCount) { $_.downloads - $lastCount } else { 0 }
  $lastCount = $_.downloads
  [pscustomobject]@{timestamp = $_.timestamp; downloadsRel = $delta}
}

writeData $nugetByDay "$CookedDir/nugetdeltabyday.csv" $false

# Uploading cooked.
if ($UploadCooked) {
  Write-Verbose "Uploading cooked data to $BlobCookedUrl"
  & $azcopy /y /DestKey:$BlobStorageKey /Source:$CookedDir /s /Dest:$BlobCookedUrl /xo
  if ($LASTEXITCODE -ne 0) {
    throw "azcopy operation for uploading cooked logs failed"
  }
}

$nugetByDay
