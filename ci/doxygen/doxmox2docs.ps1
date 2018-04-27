#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#

[CmdletBinding()]Param(
  [Parameter(Mandatory=$true)]
  [ValidateScript({ Test-Path $_ })]
  [string]$DoxygenIndexXml,

  [Parameter(Mandatory=$true)]
  [ValidateScript({ Test-Path $_ })]
  [string]$MoxygenApiMd,

  [Parameter(Mandatory=$true)]
  [ValidateScript({ -not (Test-Path $_) })]
  [string]$OutputDir
)

$ErrorActionPreference = 'Stop'

New-Item -Type Directory $OutputDir

$index = [xml](Get-Content $DoxygenIndexXml)

$namespaceRoot = 'Microsoft::CognitiveServices::Speech'

# TODO check for clashes?
# TODO generate TOC; class by namespace?

function getFilename($compound) {
  if ($compound.kind -eq 'namespace') {
    ($compound.name.ToLower() -replace '::', '-') + '-namespace.md'
  } else { # struct or class
    ($compound.name.Substring($namespaceRoot.Length + 2).ToLower() -replace '::', '-') + '.md'
  }
}

# Dictionary: refid -> markdown
$linkTarget = @{}

foreach ($compound in $index.doxygenindex.compound) {
  switch -Regex ($compound.kind) {
    "^(?:class|namespace|struct)$" {
      if (($compound.name -like "$namespaceRoot::*") -or
        ($compound.kind -eq 'namespace' -and $compound.name -eq $namespaceRoot)) {
        $link = getFilename $compound
        Write-Debug ("refid {0} - {1} {2} -> {3}" -f $compound.refid, $compound.kind, $compound.name, $link)
        $linkTarget.Add($compound.refid, $link)

        foreach ($member in $compound.member) {
          switch -Regex ($member.kind) {
            "class" { }
            "^(?:friend|function|typedef|variable|enum|enumvalue|typedef)$" {
              # Some kinds we're using:
              #   For class:
              #     friend function typedef variable
              #   For namespace:
              #     enum enumvalue typedef
              if ($member.refid -like "$($compound.refid)_*") {
                # TODO implement refid shortening
                $linkTarget.Add($member.refid, $link + '#' + $member.refid)
              } elseif ($member.refid -like "dummy_*") {
                # Ignore.
              } else {
                # TODO only if INLINE_INHERITED_MEMB = YES ?
                #Write-Warning "Unexpected ref ID $($member.refid) within ref ID $($compound.refid)"
              }
            }
            default { Write-Warning "Unhandled member kind $($member.kind)" }
          }
        }
      } else {
        Write-Warning "Unhandled class $($compound.name)"
      }
    }
    "^(?:file|page)$" {
      if ($compound.member -eq $null) {
        Write-Warning "Ignoring non-empty $($compound.kind) compound $($compound.name)"
      }
    }
    default { Write-Warning "Unhandled compound kind $($compound.kind)" }
  }
}

function fixLinks($line) {
  $stringBuilder = New-Object System.Text.StringBuilder

  $pattern = "{#(?<def>[^)}]+)}|\(#(?<ref>[^)}]+)\)"
  $m = [regex]::Match($line, $pattern)
  $lastPos = 0

  while ($m.Success) {
    # Append the non-matching portion
    $stringBuilder.Append($line.Substring($lastPos, $m.Index - $lastPos)) | Out-Null

    if ($m.Groups['ref'].Success) {
      $ref = $m.Groups['ref'].Value
      $stringBuilder.Append('(') | Out-Null
      if (-not $linkTarget.ContainsKey($ref)) {
        Write-Warning "Unknown ref $ref"
        $stringBuilder.Append("unknow-REF") | Out-Null
      } else {
        $stringBuilder.Append($linkTarget[$ref]) | Out-Null
      }
      $stringBuilder.Append(')') | Out-Null
    } else {
      $def = $m.Groups['def'].Value
      $stringBuilder.Append('<a name="') | Out-Null
      $stringBuilder.Append($def) | Out-Null
      $stringBuilder.Append('"></a>') | Out-Null
    }
    $lastPos = $m.Index + $m.Value.Length
    $m = $m.NextMatch()
  }
  # Append final non-matching portion
  $stringBuilder.Append($line.Substring($lastPos)) | Out-Null

  $stringBuilder.ToString()
}

$stream = $null
try {

  $currentRefId = $null
  $currentFile = $null
  # Fairly small  file...
  foreach ($line in (Get-Content -Encoding UTF8 $MoxygenApiMd)) {
    switch -Regex ($line) {
      "^MOXYGENBEGIN {#(?<refid>.*)}" {
        if ($currentRefId -ne $null) {
          Write-Error "Unexpected $line"
        }
        $currentRefId = $matches.refid
        if (-not ($linkTarget.Contains($currentRefId))) {
          Write-Error "Unknown ref ID $currentRefId"
        }
        $file = $linkTarget[$currentRefId]
        # TODO open file etc.
        $stream = [System.IO.StreamWriter] (Join-Path $OutputDir $file)
      }
      "^MOXYGENEND {#(?<refid>.*)}" {
        if ($matches.refid -ne $currentRefId) {
          Write-Error "Unexpected $line"
        }
        $stream.close()
        $currentRefId = $null
      }
      default {
        if ($currentRefId -ne $null) {
          #$line = $line -replace '{#(\S*)}', '<a name="${1}"></a>'
          #$line = $line -replace '{#(\S*)}', '<a name="${1}"></a>'
          #"{0} - {1}" -f $file, $line
          $stream.WriteLine((fixLinks $_))
        }
      }
    }
  }
} finally {
  if ($stream -ne $null) {
    $stream.close()
  }
}
