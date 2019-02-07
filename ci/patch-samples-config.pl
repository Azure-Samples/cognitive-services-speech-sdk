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
  use warnings;
  use strict;

  @ARGV == 13 or die "Expecting exactly 13 arguments.\n";

  our ($samplesDir, $audioFile, $speechKey, $speechRegion, $speechEndpointId, $luisKey, $luisRegion, $luisAppId, $luisIntent1, $luisIntent2, $luisIntent3, $keywordRecognitionModelFile, $keyword) = splice @ARGV;

  -d $samplesDir or die "Cannot find $samplesDir\n";

  # Translate to full native path (i.e., Windows path on Cygwin / Git Bash),
  # with escaping for use in strings of C-like languages and Perl substitute
  # replacement.
  sub getNativeEscapedPath {
      my $path = shift;
      my $nativePath;

      # The next one is a warning only for scenarios in which patching is done on the
      # host, but test may run in a virtual environment (e.g., container) under a
      # different path.
      -f $path or warn "Cannot find $path\n";

      if (exists $ENV{OS} and $ENV{OS} eq 'Windows_NT') {
        $nativePath = `cygpath -aw "$path"`;
        chomp $nativePath;
      } else {
        $nativePath = $path;
      }
      return $nativePath =~ s/\\/\\\\\\\\/gr;
  }

  our $escapedAudioFile = getNativeEscapedPath($audioFile);
  our $escapedKeywordRecognitionModelFile = getNativeEscapedPath($keywordRecognitionModelFile);

  @ARGV = ();
  find(sub {
    m(\.(?:cpp|cs|html|ipynb|java|js|m|md|php|py)$) &&
    push @ARGV, $File::Find::name
  }, $samplesDir);
}
if ($ARGV ne $oldargv) {
  warn "Patching $ARGV\n";
  $oldargv = $ARGV;
}

if ($ARGV =~ m(\.md$)) {
  # In Markdown we replace with dummies. After running this script,
  #   grep -rP '\bYour[A-Z]\w+' <samples-dir>
  # should turn out empty...
  s(\b(?:
    YourAudioFile |
    YourEndpointId |
    YourKeyword |
    YourKeywordRecognitionModelFile |
    YourLanguageUnderstandingAppId |
    YourLanguageUnderstandingIntentName1 |
    YourLanguageUnderstandingIntentName2 |
    YourLanguageUnderstandingIntentName3 |
    YourLanguageUnderstandingServiceRegion |
    YourLanguageUnderstandingSubscriptionKey |
    YourServiceRegion |
    YourSubscriptionKey
  ))(SOMETHING)gx;
} else {
  s("YourAudioFile\.wav")("$escapedAudioFile");
  s((["'])YourSubscriptionKey\1)(\1$speechKey\1)g;
  s((["'])YourServiceRegion\1)(\1$speechRegion\1)g;
  s("YourEndpointId")("$speechEndpointId")g;
  s("YourLanguageUnderstandingAppId")("$luisAppId")g;
  s("YourLanguageUnderstandingSubscriptionKey")("$luisKey")g;
  s("YourLanguageUnderstandingServiceRegion")("$luisRegion")g;
  s("YourLanguageUnderstandingIntentName1")("$luisIntent1")g;
  s("YourLanguageUnderstandingIntentName2")("$luisIntent2")g;
  s("YourLanguageUnderstandingIntentName3")("$luisIntent3")g;
  s("YourKeywordRecognitionModelFile\.table")("$escapedKeywordRecognitionModelFile")g;
  s("YourKeyword")("$keyword")g;
}
