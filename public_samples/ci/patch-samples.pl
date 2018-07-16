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

  # Some REs
  $rePkgId = qr/\QMicrosoft.CognitiveServices.Speech\E/;
  $rePkgConfig = qr/packages\.config/;
  $reCsProj = qr/.*\.csproj/;
  $reVcxProj = qr/.*\.vcxproj/;

  $version = shift
    or die "Supply version to use as first and only argument\n";
  $samplesDir = '.';

  -d $samplesDir
    or die "Cannot find $samplesDir, please launch from repository root\n";

  @ARGV = ();
  find(sub {
    m(^(?:$rePkgConfig|$reCsProj|$reVcxProj)$) &&
    push @ARGV, $File::Find::name
  }, $samplesDir);
}
if ($ARGV ne $oldargv) {
  warn "Patching $ARGV\n";
  $oldargv = $ARGV;
}
$ARGV =~ m(.*/$rePkgConfig$) && s/(<package id="$rePkgId" version=")([^"]*)"/$1$version"/;
$ARGV =~ m(.*/(?:$reCsProj|$reVcxProj)$) && do {
  # Custom version property we have in smoke check
  s((\Q<SpeechSdkVersion Condition="'$(SpeechSdkVersion)' == ''">\E)([^<]*)(\Q</SpeechSdkVersion>\E))($1$version$3)g;

  # <HintPath>...<HintPath>, <Import Project ... />, <Error ... />
  s((["'>](?:\.\.\\)*packages\\$rePkgId\.)[^\\]*\\)($1$version\\)g;
};
