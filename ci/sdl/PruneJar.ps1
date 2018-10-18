#
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
#

[CmdletBinding()]Param(
  [Parameter(Mandatory=$true)]
  [ValidateScript({ Test-Path $_ })]
  [string]$InputJarName, # the SourceFile (including path)
  
  [Parameter(Mandatory=$true)]
  [string]$OutputDir     # location where the extracted and pruned jar subtree out of the source file will reside
)
$ErrorActionPreference = 'Stop'
$Hierarchy = "com\microsoft\cognitiveservices\speech\internal"

# we need an empty OutputDir
Remove-Item $OutputDir -Recurse -Force -ErrorAction SilentlyContinue | Out-Null
New-Item -Type Directory $OutputDir -Force -ErrorAction SilentlyContinue | Out-Null

#copy inputfile into file with zip extension, extract into temp directory, copy the subtree and delete the extracted files
$InputZipName = Join-Path $OutputDir "PruneJar.zip"
$UnzipIntoDir = Join-Path $OutputDir "PruneJarDir.tmp"

Copy-Item $InputJarName $InputZipName -Force | Out-Null
Expand-Archive $InputZipName -DestinationPath $UnzipIntoDir -Force | Out-Null

# copy subdirectories from unzip into the zip directory
Copy-Item (Join-Path $UnzipIntoDir $Hierarchy) -Destination (Join-Path $OutputDir $Hierarchy) -Recurse -Force | Out-Null

Remove-Item $InputZipName -Force -ErrorAction SilentlyContinue | Out-Null
Remove-Item $UnzipIntoDir -Recurse -Force -ErrorAction SilentlyContinue | Out-Null
