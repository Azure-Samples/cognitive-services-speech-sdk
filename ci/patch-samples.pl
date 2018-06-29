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
  $public_samples = 'public_samples';

  -d $public_samples
    or die "Cannot find $public_samples directory, please launch from repository root\n";

  @ARGV = ();
  find(sub {
    m(^(?:$rePkgConfig|$reCsProj|$reVcxProj)$) &&
    push @ARGV, $File::Find::name
  }, $public_samples);
}
if ($ARGV ne $oldargv) {
  warn "Patching $ARGV\n";
  $oldargv = $ARGV;
}
$ARGV =~ m(.*/$rePkgConfig$) && s/(<package id="$rePkgId" version=")([^"]*)"/$1$version"/;
$ARGV =~ m(.*/(?:$reCsProj|$reVcxProj)$) && do {
  # <HintPath>...<HintPath>, <Import Project ... />, <Error ... />
  s((["'>](?:\.\.\\)*packages\\$rePkgId\.)[^\\]*\\)($1$version\\)g;
};
