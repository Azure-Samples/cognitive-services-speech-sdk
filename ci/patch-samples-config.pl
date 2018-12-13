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

  @ARGV == 11 or die "Expecting exactly 11 arguments.\n";

  our ($samplesDir, $audioFile, $speechKey, $speechRegion, $speechEndpointId, $luisKey, $luisRegion, $luisAppId, $luisIntent1, $luisIntent2, $luisIntent3) = splice @ARGV;

  -d $samplesDir or die "Cannot find $samplesDir\n";
  # The next is a warning only for scenarios in which patching is done on the
  # host, but test may run in a virtual environment (e.g., container) under a
  # different path.
  -f $audioFile or warn "Cannot find $audioFile\n";

  # Translate to full Windows path, with escaping for use in
  # strings of C-like languages and Perl substitute replacement.
  my $nativePath;
  if (exists $ENV{OS} and $ENV{OS} eq 'Windows_NT') {
    $nativePath = `cygpath -aw "$audioFile"`;
    chomp $nativePath;
  } else {
    $nativePath = $audioFile;
  }
  our $escapedAudioFile = $nativePath =~ s/\\/\\\\\\\\/gr;

  @ARGV = ();
  find(sub {
    m(\.(?:cpp|js|cs|html|java|m|php|md)$) &&
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
    YourLanguageUnderstandingAppId |
    YourLanguageUnderstandingIntentName1 |
    YourLanguageUnderstandingIntentName2 |
    YourLanguageUnderstandingIntentName3 |
    YourLanguageUnderstandingServiceRegion |
    YourLanguageUnderstandingSubscriptionKey |
    YourServiceRegion |
    YourSubscriptionKey |
    YourSubscriptionServiceRegion
  ))(SOMETHING)gx;
} else {
  s("YourAudioFile\.wav")("$escapedAudioFile");
  s((["'])YourSubscriptionKey\1)(\1$speechKey\1)g;
  s((["'])Your(?:Subscription)?ServiceRegion\1)(\1$speechRegion\1)g;
  s("YourEndpointId")("$speechEndpointId")g;
  s("YourLanguageUnderstandingAppId")("$luisAppId")g;
  s("YourLanguageUnderstandingSubscriptionKey")("$luisKey")g;
  s("YourLanguageUnderstandingServiceRegion")("$luisRegion")g;
  s("YourLanguageUnderstandingIntentName1")("$luisIntent1")g;
  s("YourLanguageUnderstandingIntentName2")("$luisIntent2")g;
  s("YourLanguageUnderstandingIntentName3")("$luisIntent3")g;
}
