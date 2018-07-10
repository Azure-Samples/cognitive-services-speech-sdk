#!/usr/bin/perl -i -p
# (can be launched, for example, from Git Bash prompt)
BEGIN {
  use File::Find;

  # Some REs
  $reNugetId = qr/\QMicrosoft.CognitiveServices.Speech\E/;
  $reMavenId = qr/\Qcom.microsoft.cognitiveservices.speech:client-sdk\E/;
  $rePkgConfig = qr/packages\.config/;
  $reCsProj = qr/.*\.csproj/;
  $reVcxProj = qr/.*\.vcxproj/;
  $reGradleBuild = qr/build\.gradle/;

  $version = shift
    or die "Supply version to use as first and only argument\n";
  $public_samples = 'public_samples';

  -d $public_samples
    or die "Cannot find $public_samples directory, please launch from repository root\n";

  @ARGV = ();
  find(sub {
    m(^(?:$rePkgConfig|$reCsProj|$reVcxProj|$reGradleBuild)$) &&
    push @ARGV, $File::Find::name
  }, $public_samples);

  $seenPackageReference = 0;
}
if ($ARGV ne $oldargv) {
  warn "Patching $ARGV\n";
  $oldargv = $ARGV;
}
$ARGV =~ m(.*/$rePkgConfig$) && s/(<package id="$reNugetId" version=")([^"]*)"/$1$version"/;
$ARGV =~ m(.*/(?:$reCsProj|$reVcxProj)$) && do {
  # <HintPath>...<HintPath>, <Import Project ... />, <Error ... />
  s((["'>](?:\.\.\\)*packages\\$reNugetId\.)[^\\]*\\)($1$version\\)g;

  # <PackageReference Include="Microsoft.CognitiveServices.Speech" Version="X" />
  s((?<=<PackageReference Include="$reNugetId" Version=")[^"]*)($version)g;

  # <PackageReference Include="Microsoft.CognitiveServices.Speech">\n<Version>X</Version>
  s((?<=<Version>)[^<]*)($version)g if $seenPackageReference;
  $seenPackageReference = m(<PackageReference Include="$reNugetId">);
};
$ARGV =~ m(.*/$reGradleBuild$) && s/(\bimplementation\s+(['"])$reMavenId:)(.*?)\2/$1$version$2/;
