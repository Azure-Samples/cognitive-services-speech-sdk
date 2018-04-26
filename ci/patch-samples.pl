#!/usr/bin/perl -i -p
# (can be launched, for example, from Git Bash prompt)
BEGIN {
  use File::Find;

  # Some REs
  $rePkgId = qr/\QMicrosoft.CognitiveServices.Speech\E/;
  $rePkgConfig = qr/packages\.config/;
  $reCsProj = qr/.*\.csproj/;
  $reVcxProj = qr/.*\.vcxproj/;

  $version = shift
    or die "Supply version to use as first and only argument\n";
  $samplesDir = 'samples';

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
  s((["'>]packages\\$rePkgId\.)[^\\]*\\)($1$version\\)g;
};
