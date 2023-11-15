#!/usr/bin/perl -i -p
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
# vim:set expandtab shiftwidth=2 tabstop=2:
#
# (can be launched, for example, from a Git Bash prompt)
BEGIN {
  use File::Find;

  $mavenArtifactId = "client-sdk";
  $mavenEmbeddedArtifactId = "client-sdk-embedded";

  # Some REs
  $reNugetId = qr/\QMicrosoft.CognitiveServices.Speech\E/;
  $reNugetExtensionEmbeddedSrId = qr/\QMicrosoft.CognitiveServices.Speech.Extension.Embedded.SR\E/;
  $reNugetExtensionEmbeddedTtsId = qr/\QMicrosoft.CognitiveServices.Speech.Extension.Embedded.TTS\E/;
  $reNugetExtensionOnnxRuntimeId = qr/\QMicrosoft.CognitiveServices.Speech.Extension.ONNX.Runtime\E/;
  $reNugetExtensionTelemetryId = qr/\QMicrosoft.CognitiveServices.Speech.Extension.Telemetry\E/;
  $reNugetExtensionRemoteConversation = qr/\QMicrosoft.CognitiveServices.Speech.Remoteconversation\E/;
  $reNugetExtensionMasId = qr/\QMicrosoft.CognitiveServices.Speech.Extension.MAS\E/;
  $reMavenGroupId = qr/\Qcom.microsoft.cognitiveservices.speech\E/;
  $reMavenArtifactId = qr/\Q$mavenArtifactId\E/;
  $reMavenEmbeddedArtifactId = qr/\Q$mavenEmbeddedArtifactId\E/;
  $reMavenId = qr/$reMavenGroupId:$reMavenArtifactId/;
  $reMavenEmbeddedId = qr/$reMavenGroupId:$reMavenEmbeddedArtifactId/;
  $rePkgConfig = qr/packages\.config/;
  $reCsProj = qr/.*\.csproj/;
  $reVcxProj = qr/.*\.vcxproj/;
  $reGradleBuild = qr/build\.gradle/;
  $reMavenPom = qr/pom\.xml/;

  $version = shift
    or die "Supply version to use as first argument\n";
  $samplesDir = shift
    or die "Supply directory to use as second argument\n";

  # Optional, force using an embedded (& cloud) speech package in all
  # samples where a cloud-only package would be normally used. This is
  # specifically needed in embedded speech CI jobs where cloud samples
  # are tested. Only affects Java JRE samples (with pom.xml) for now.
  $forceEmbeddedArtifactId = shift // false;

  # Note: revision for file/assembly version (8 local, 12 dev, 16 int, 25..28
  #       prod alpha/beta/rc/release)
  $version =~ qr(
    ^
    (?<mmp>\d+\.\d+\.\d+)
    (?:
      (?<rev8>-alpha\.0\.1) |
      (?<rev12>-alpha\.0\.\d+) |
      (?<rev16>-beta\.0\.\d+) |
      (?<rev25>-alpha\.[1-9]\d*) |
      (?<rev26>-beta\.[1-9]\d*) |
      (?<rev27>-rc\.[1-9]\d*) |
      (?<rev28>)
    )
    $
  )x or die "Unexpected version number $version\n";

  # Parse out revision number from capture group rev*
  $rev = substr((grep { $_ ne 'mmp' } keys %+)[0], 3);
  $assemblyVersion = "$+{mmp}.$rev";

  -d $samplesDir
    or die "Cannot find $samplesDir\n";

  @ARGV = ();
  find(sub {
    m(^(?:$rePkgConfig|$reCsProj|$reVcxProj|$reGradleBuild|$reMavenPom)$) &&
    push @ARGV, $File::Find::name
  }, $samplesDir);

  $seenPackageReference = 0;
  $mavenDepState = 0;
}
if ($ARGV ne $oldargv) {
  warn "Patching $ARGV\n";
  $oldargv = $ARGV;
}
$ARGV =~ m(.*/$rePkgConfig$) && s/(<package id="$reNugetId.*" version=")([^"]*)"/$1$version"/;
$ARGV =~ m(.*/$rePkgConfig$) && s/(<package id="$reNugetExtensionRemoteConversation.*" version=")([^"]*)"/$1$version"/;
$ARGV =~ m(.*/(?:$reCsProj|$reVcxProj)$) && do {
  # <HintPath>...<HintPath>, <Import Project ... />, <Error ... />
  if (m($reNugetExtensionEmbeddedSrId)) {
    s((["'>](?:\.\.\\)*packages\\$reNugetExtensionEmbeddedSrId\.)[^\\]*\\)($1$version\\)g;
  } elsif (m($reNugetExtensionEmbeddedTtsId)) {
    s((["'>](?:\.\.\\)*packages\\$reNugetExtensionEmbeddedTtsId\.)[^\\]*\\)($1$version\\)g;
  } elsif (m($reNugetExtensionOnnxRuntimeId)) {
    s((["'>](?:\.\.\\)*packages\\$reNugetExtensionOnnxRuntimeId\.)[^\\]*\\)($1$version\\)g;
  } elsif (m($reNugetExtensionTelemetryId)) {
    s((["'>](?:\.\.\\)*packages\\$reNugetExtensionTelemetryId\.)[^\\]*\\)($1$version\\)g;
  } elsif (m($reNugetExtensionRemoteConversation)) {
    s((["'>](?:\.\.\\)*packages\\$reNugetExtensionTelemetryId\.)[^\\]*\\)($1$version\\)g;
  } elsif (m($reNugetExtensionMasId)) {
    s((["'>](?:\.\.\\)*packages\\$reNugetExtensionMasId\.)[^\\]*\\)($1$version\\)g;
  } else {
    s((["'>](?:\.\.\\)*packages\\$reNugetId\.)[^\\]*\\)($1$version\\)g;
  }

  # Assembly reference
  s((?<=<Reference Include="$reNugetId\.csharp, Version=)\d+\.\d+\.\d+\.\d+)($assemblyVersion)g;

  # <PackageReference Include="Microsoft.CognitiveServices.Speech" Version="X" />
  s((?<=<PackageReference Include="$reNugetId" Version=")[^"]*)($version)g;
  s((?<=<PackageReference Include="$reNugetExtensionEmbeddedSrId" Version=")[^"]*)($version)g;
  s((?<=<PackageReference Include="$reNugetExtensionEmbeddedTtsId" Version=")[^"]*)($version)g;
  s((?<=<PackageReference Include="$reNugetExtensionOnnxRuntimeId" Version=")[^"]*)($version)g;
  s((?<=<PackageReference Include="$reNugetExtensionTelemetryId" Version=")[^"]*)($version)g;
  s((?<=<PackageReference Include="$reNugetExtensionRemoteConversation" Version=")[^"]*)($version)g;
  s((?<=<PackageReference Include="$reNugetExtensionMasId" Version=")[^"]*)($version)g;

  # <PackageReference Include="Microsoft.CognitiveServices.Speech">\n<Version>X</Version>
  s((?<=<Version>)[^<]*)($version)g if $seenPackageReference;
  $seenPackageReference = m(<PackageReference Include="$reNugetId">);
};
$ARGV =~ m(.*/$reGradleBuild$) && s/(\bimplementation\s+(['"])($reMavenId|$reMavenEmbeddedId):)(.*?)\2/$1$version$2/;
$ARGV =~ m(.*/$reMavenPom$) && do {
  if ($mavenDepState == 2) {
    $mavenDepState = 0 if s((?<=<version>)[^<]*)($version);
  } elsif ($mavenDepState == 1) {
    if (m(<artifactId>($reMavenArtifactId|$reMavenEmbeddedArtifactId)</artifactId>)) {
      $mavenDepState = 2;
      if ($forceEmbeddedArtifactId eq true) {
        s((?<=<artifactId>)$mavenArtifactId(?=</artifactId>))($mavenEmbeddedArtifactId);
      }
    }
  } else {
    $mavenDepState = 1 if m(<groupId>$reMavenGroupId</groupId>);
  }
}
