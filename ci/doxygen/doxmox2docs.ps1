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

New-Item -Type Directory $OutputDir | Out-Null

$OutputDirAbsolute = Resolve-Path $OutputDir

$index = [xml](Get-Content $DoxygenIndexXml)

$namespaceRoot = 'Microsoft::CognitiveServices::Speech'

# TODO check for clashes?
# TODO generate TOC; class by namespace?
# TODO see also links

function getFilename($compound) {
  if ($compound.kind -eq 'namespace') {
    ($compound.name.ToLower() -replace '::', '-') + '-namespace.md'
  } else { # struct or class
    ($compound.name.Substring($namespaceRoot.Length + 2).ToLower() -replace '::', '-') + '.md'
  }
}

# TODO friends?

function getIndexEntryForFile($compound) {
  $kind = $compound.kind;
  $name = $compound.name;
  if ($compound.kind -eq 'namespace') {
    $file = ($compound.name.ToLower() -replace '::', '-') + '-namespace.md'
    $level = 1
    $namespace = $compound.name
  } else { # struct or class
    $file = ($compound.name.Substring($namespaceRoot.Length + 2).ToLower() -replace '::', '-') + '.md'
    $level = 2
    $compound.name -match '^(.*)::' | Out-Null
    $namespace = $matches[1]
  }
  $compound.name -match '([^:]*)$' | Out-Null
  $lastName = $matches[1]

  [pscustomobject]@{
    Anchor = $null
    File = $file
    Kind = $kind
    Level = $level
    Namespace = $namespace
    Name = $name
    LastName = $lastName
  }
}

# Dictionary: refid -> markdown
$ourIndex = @{indexref = [pscustomobject]@{
  Anchor = $null
  File = 'index.md'
  Kind = 'index'
  Level = -1
  Name = 'index'
  Namespace = ''
}}

foreach ($compound in $index.doxygenindex.compound) {
  switch -Regex ($compound.kind) {
    "^(?:class|namespace|struct)$" {
      if (($compound.name -like "$namespaceRoot::*") -or
        ($compound.kind -eq 'namespace' -and $compound.name -eq $namespaceRoot)) {

        $indexEntry = getIndexEntryForFile $compound
        Write-Debug ("refid {0} - {1} {2} -> {3}" -f $compound.refid, $compound.kind, $compound.name, $link)
        # TODO check for clashes. anchors for special names (operator overloads etc.)
        $ourIndex.Add($compound.refid, $indexEntry)
        foreach ($member in $compound.member) {
          switch -Regex ($member.kind) {
            "class" { }
            "^(?:friend|function|typedef|variable|enum|enumvalue|typedef)$" {
              # Some kinds we're using:
              #   For class:
              #     friend function typedef variable
              #   For namespace:
              #     enum enumvalue typedef
              if (($member.refid -like "$($compound.refid)_*") -or
                  ($compound.kind -eq 'namespace' -and $member.kind -eq 'enum')) {
                # TODO implement refid shortening
                $memberIndexEntry = $indexEntry.PsObject.Copy()
                #$ourIndex.Add($member.refid, $link + '#' + $member.refid)
                #$memberIndexEntry.Anchor = '#' + $member.refid
                $memberIndexEntry.Anchor = '#' + $member.name.ToLower()
                $memberIndexEntry.Level++
                $memberIndexEntry.Kind = $member.kind
                $ourIndex.Add($member.refid, $memberIndexEntry)
              } elseif ($member.refid -like "dummy_*") {
                # Ignore.
              } elseif ($member.kind -eq "typedef") {
                # Ignore.
              } else {
                # TODO many more if INLINE_INHERITED_MEMB = YES
                Write-Warning "Unexpected ref ID $($member.refid) within ref ID $($compound.refid)"
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
      if ($compound.member -ne $null) {
        Write-Warning "Ignoring non-empty $($compound.kind) compound $($compound.name)"
      }
    }
    default { Write-Warning "Unhandled compound kind $($compound.kind)" }
  }
}

function fixLinks($line, $file) {
  $stringBuilder = New-Object System.Text.StringBuilder

  $pattern = "{#(?<def>[^)}]+)}|\[(?<refText>.*?)\]\(#(?<refTarget>[^)}]+)\)"
  $m = [regex]::Match($line, $pattern)
  $lastPos = 0

  while ($m.Success) {
    # Append the non-matching portion
    $stringBuilder.Append($line.Substring($lastPos, $m.Index - $lastPos)) | Out-Null

    if ($m.Groups['refTarget'].Success) {
      $refTarget = $m.Groups['refTarget'].Value

      if ($ourIndex.ContainsKey($refTarget)) {
        $indexEntry = $ourIndex[$refTarget]
        $removeLink = ($indexEntry.File -eq $file) -and -not $indexEntry.Anchor
      } else {
        $removeLink = $true
        Write-Warning "Unknown reference to $refTarget in file $file"
      }
      if ($removeLink) {
        $stringBuilder.Append($m.Groups['refText']) | Out-Null
      } else {
        $stringBuilder.Append('[') | Out-Null
        $stringBuilder.Append($m.Groups['refText']) | Out-Null
        $stringBuilder.Append(']') | Out-Null
        $stringBuilder.Append('(') | Out-Null
        if ($indexEntry.File -ne $file) {
          $stringBuilder.Append($indexEntry.File) | Out-Null
        }
        if ($indexEntry.Anchor) {
          $stringBuilder.Append($indexEntry.Anchor) | Out-Null
        }
        $stringBuilder.Append(')') | Out-Null
      }
    } else {
      $def = $m.Groups['def'].Value

      if (-not $ourIndex.ContainsKey($def)) {
        Write-Error "Unexpected reference definition for $def from file $file"
      } else {
        $indexEntry = $ourIndex[$def]
        if ($indexEntry.Anchor) {
          $stringBuilder.Append('<a name="') | Out-Null
          # (Skip the initial hash-tag)
          $stringBuilder.Append($indexEntry.Anchor.Substring(1)) | Out-Null
          $stringBuilder.Append('"></a>') | Out-Null
        }
      }
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
        if (-not ($ourIndex.Contains($currentRefId))) {
          Write-Error "Unknown ref ID $currentRefId"
        }
        $file = $ourIndex[$currentRefId].File
        # TODO open file etc.
        $stream = [System.IO.StreamWriter] (Join-Path $OutputDirAbsolute $file)
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
          #$line = $line -replace "`r$", "";
          $line = fixLinks $_ $file;
          # TODO hacky
          if ($line -notmatch "namespace $namespaceRoot::") {
            $line = $line -replace "$namespaceRoot::", '';
          }
          $stream.WriteLine($line)
        }
      }
    }
  }
} finally {
  if ($stream -ne $null) {
    $stream.close()
  }
}

# Generate TOC
$stream = $null
try {
  $stream = [System.IO.StreamWriter] (Join-Path $OutputDirAbsolute 'TOC.md')

  $header = @'
# [Speech SDK for C++ Reference](index.md)
'@
  $stream.WriteLine($header)
  $ourIndex.Values |
    Sort-Object Namespace, Level, Name |
    Where-Object Level -lt 3 |
    Where-Object Level -gt 0 |
    Where-Object Kind -ne 'enum' |
    ForEach-Object {
      $stream.WriteLine(
        ('{0} [{1} {2}]({3})' -f
          ('#' * ($_.Level + 1)),
          $_.LastName,
          $_.Kind,
          $_.File))
      Write-Debug $_
    }
} finally {
  if ($stream -ne $null) {
    $stream.close()
  }
}
