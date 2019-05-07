[CmdletBinding()]
Param(
  # Raw data from Azure Blob $logs container, past 30 days at most.
  [string]
  [Parameter(Mandatory = $true)]
  [ValidateScript({ Test-Path -PathType Container -LiteralPath $_})]
  $RawDir,

  # Cooked / scrubbed output data (no client IP).
  [string]
  [Parameter(Mandatory = $true)]
  [ValidateScript({ Test-Path -PathType Container -LiteralPath $_})]
  $CookedDir,

  # Today's date (allow to override for test purposes)
  [datetime]
  $Today = [datetime]::UtcNow.Date,

  # How many days to look back from today for raw logs
  [int]
  $LookBackDays = 7,

  # Maximum allowed misses in window.
  [int]
  $MaximumMisses = 1,

  [switch]
  $DownloadRaw,

  [switch]
  $DownloadCooked,

  [switch]
  $UploadCooked,

  [string]
  $BlobStorageKey,

  [string]
  $BlobRawUrl = 'https://csspeechstorage.blob.core.windows.net/$logs',

  [string]
  $BlobCookedUrl = 'https://csspeechstorage.blob.core.windows.net/usagedata/blob',

  [switch]
  $Force
)

$azcopy = "c:\Program Files (x86)\Microsoft SDKs\Azure\AzCopy\AzCopy.exe"
$needBlobOperation = $DownloadCooked -or $DownloadRaw -or $UploadCooked
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
$RawDir = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($RawDir)
$CookedDir = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($CookedDir)

if ($RawDir.StartsWith($CookedDir) -or $CookedDir.StartsWith($RawDir)) {
  throw "Raw dir should not be contained in cooked dir, or vice versa: $RawDir vs. $CookedDir"
}

# Downloading cooked before raw to reduce exposure.
if ($DownloadCooked) {
  Write-Verbose "Downloading cooked data from $BlobCookedUrl"
  & $azcopy /y /SourceKey:$BlobStorageKey /Source:$BlobCookedUrl /s /Dest:$CookedDir /mt /xo
  if ($LASTEXITCODE -ne 0) {
    throw "azcopy operation for downloading cooked logs failed"
  }
}

if ($DownloadRaw) {
  Write-Verbose "Downloading current raw data from $BlobRawUrl"
  & $azcopy /y /SourceKey:$BlobStorageKey /Source:$BlobRawUrl /s /Dest:$RawDir /mt /xo
  if ($LASTEXITCODE -ne 0) {
    throw "azcopy operation for downloading raw logs failed"
  }
}

# TODO versioning for our own data ?

# Schema for Azure Blob Storage Logs, version 1.0. See
# https://docs.microsoft.com/rest/api/storageservices/storage-analytics-log-format
# for details.
#
# Note: we will only accept version-number 1.0 below.
#
# Note: the following remark also holds for our outpus: "Any field that may
# contain a quote, a semicolon, or a newline (\n) is HTML encoded and quoted."
$logFormatVersion = '1.0'
$rawColumns = @(
  "version-number", "request-start-time", "operation-type", "request-status",
  "http-status-code", "end-to-end-latency-in-ms", "server-latency-in-ms",
  "authentication-type", "requester-account-name", "owner-account-name",
  "service-type", "request-url", "requested-object-key", "request-id-header",
  "operation-count", "requester-ip-address", "request-version-header",
  "request-header-size", "request-packet-size", "response-header-size",
  "response-packet-size", "request-content-length", "request-md5", "server-md5",
  "etag-identifier", "last-modified-time", "conditions-used", "user-agent-header",
  "referrer-header", "client-request-id")

# Columns to remove
$excludeRawColumns = @('requester-ip-address')

$cookedColumns = $rawColumns | Where-Object { $_ -notin $excludeRawColumns }

function writeData($data, [string]$path, [bool]$isOld) {

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
    # Sanity check - old data must be stable, recent data may only increase.
    $direction = [math]::Sign((Get-Item -LiteralPath $newData).Length -
      (Get-Item -LiteralPath $path).Length)
    if ($direction -eq 0) {
      # If hash also matches reset $dataChanged
      if ((Get-FileHash -Algorithm SHA256 -LiteralPath $newData).Hash -eq
        (Get-FileHash -Algorithm SHA256 -LiteralPath $path).Hash) {
        $dataChanged = $false
      }
    }
    if ($isOld -and $dataChanged) {
      Remove-Item -Force -LiteralPath $newData -ErrorAction SilentlyContinue
      throw "Different data than we had for old log"
    } elseif ($direction -eq -1) {
      Remove-Item -Force -LiteralPath $newData -ErrorAction SilentlyContinue
      throw "Less data than we had for recent log"
    }
    if ($direction -eq 0) {
      if ($dataChanged) {
        Write-Verbose "  $path -- length unchanged, content different."
      } else {
        Write-Verbose "  $path -- length unchanged, content the same."
      }
    } elseif ($direction -eq 1) {
      Write-Verbose "  $path -- length increased."
    }
  }

  if ($dataChanged) {
    Move-Item -LiteralPath $newData -Destination $path -Force
  } else {
    Remove-Item -Force -LiteralPath $newData -ErrorAction SilentlyContinue
  }
}

# Cook recent data

$oldDate = $today.AddDays(-2)
$day = $today.AddDays(-$LookBackDays)
$missedDays = 0
while ($day -le $Today) {
  $rawDayDir = "{0}/blob/{1:yyyy}/{1:MM}/{1:dd}" -f $RawDir, $day
  $outputDay = "{0}/bydate/{1:yyyy/MM/yyyy-MM-dd}.csv" -f $CookedDir, $day
  Write-Verbose "Cooking $day"
  if (Test-Path -PathType Container $rawDayDir) {
    $logs = @(Get-ChildItem -LiteralPath $rawDayDir -Filter *.log -File -Recurse)
    $logsCount = $logs.Length
    $records = @(Import-Csv -Path $logs.FullName -Header $rawColumns -Delimiter ';' |
      Select-Object -Property $cookedColumns)
    $rawCount = $records.Length
    $versionNumbers = @($records | Group-Object 'version-number')

    if ($versionNumbers.Name -ne ($logFormatVersion)) {
      throw "Azure Blob Storage Logs schema update! Version(s): $($versionNumbers.Name), expected only '1.0'"
    }

    # Only keep anonymous
    $records = $records | Where-Object 'authentication-type' -eq 'anonymous'
    $anonCount = $records.Length

    Write-Verbose "  $logsCount logs / $rawCount raw, $anonCount anonymous records"

    writeData $records $outputDay ($day -le $oldDate)

  } else {
    Write-Verbose "  No data"
    $missedDays++
  }
  $day = $day.AddDays(1)
}

if ($MaximumMisses -and $MaximumMisses -lt $missedDays) {
  throw "Maximum misses exceeded when cooking; maximum $MaximumMisses, actual $missedDays."
}

# Produce overall

$allCooked = @(Get-ChildItem -LiteralPath "$CookedDir/bydate" -Filter *.csv -File -Recurse)
$all = @(Import-Csv -Encoding UTF8 -Path $allCooked.FullName)
$allCount = $all.Length
$all = $all |
  Where-Object { $_.'operation-type' -eq 'GetBlob' -and $_.'request-status' -eq 'AnonymousSuccess' } |
  Sort-Object 'request-start-time'
Write-Verbose "Cooked overall: $($allCooked.Length) day(s), $allCount records, $($all.Length) GetBlob AnonymousSuccess"

writeData $all "$CookedDir/getblobanonsuccess.csv" $false

# Uploading cooked.
if ($UploadCooked) {
  Write-Verbose "Uploading cooked data to $BlobCookedUrl"
  & $azcopy /y /DestKey:$BlobStorageKey /Source:$CookedDir /s /Dest:$BlobCookedUrl /xo
  if ($LASTEXITCODE -ne 0) {
    throw "azcopy operation for uploading cooked logs failed"
  }
}

# Summarize the data
$computeUrlColumn = @{
  Name = 'Url'
  Expression = {
    ([uri](([system.net.webutility]::UrlDecode($_.'request-url')))).AbsolutePath
  }
}

$computeWeekColumn = @{
  Name = 'Week';
  Expression = {
    $date = ([datetime]($_.'request-start-time')).Date
    $date.AddDays(-[int]$date.DayOfWeek)
  }
}

$all |
  Select-Object -Property $computeWeekColumn, $computeUrlColumn |
  Group-Object Week, Url |
  Select-Object Count, @{Name = 'Week'; Expression = { $_.Group[0].Week } } , @{Name = 'Url'; Expression = { $_.Group[0].Url } } |
  Sort-Object Week, Url |
  Format-Table
