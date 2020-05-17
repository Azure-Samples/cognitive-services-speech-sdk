#!/usr/bin/perl -n
BEGIN {
  use strict;
  use warnings 'all';
  use File::Find;

  my $dir = shift
    or die "Supply sample directory to use as first argument\n";
  $#ARGV == -1
    or die "Unexpected argument(s): @ARGV\n";

  chdir $dir or die "?Cannot change directory\n";

  find(sub {
    m(^(?:.*\.(?:java|js|cpp|cs|m|html|xaml|py)|pom\.xml)$) &&
    push @ARGV, substr($File::Find::name, 2);
  }, '.');

  our $c_s = qr(^\s*// <(\w+)>);
  our $c_e = qr(^\s*// </(\w+)>);
  our $xml_s = qr(^\s*<!-- <(\w+)> -->);
  our $xml_e = qr(^\s*<!-- </(\w+)> -->);
  our $py_s = qr(^\s*# <(\w+)>);
  our $py_e = qr(^\s*# </(\w+)>);
  our $err = 0;

  our @expectedFiles;

  # Lists of files that are referenced (included) from azure-docs.
  # Note: case must match the one in the repository.
  #
  # IMPORTANT - before updating this list talk to wolfma/zhouwang/mahilleb.
  @expectedFiles = map qw(
    quickstart/java/android/from-microphone/app/src/main/res/layout/activity_main.xml
    quickstart/js/browser/token.php
    quickstart/objectivec/ios/from-microphone/helloworld/helloworld/Base.lproj/Main.storyboard
    quickstart/objectivec/macos/from-microphone/helloworld/Podfile
    quickstart/swift/ios/from-microphone/helloworld/Podfile
    quickstart/swift/macos/from-microphone/helloworld/Podfile
    quickstart/java/android/text-to-speech/app/src/main/res/layout/activity_main.xml
    quickstart/objectivec/ios/text-to-speech/helloworld/Podfile
    quickstart/objectivec/macos/text-to-speech/helloworld/Podfile
    quickstart/swift/ios/text-to-speech/helloworld/Podfile
    quickstart/swift/macos/text-to-speech/helloworld/Podfile
  );

  my @expectedTags;

  {
    no warnings 'qw'; # suppress warnings about comment in qw() below

    # Sorted list of files + tags that are referenced (included) from azure-docs
    # IMPORTANT - before updating this list talk to wolfma/zhouwang/mahilleb.
    @expectedTags = map { m/^([^#]+)#([^#]+)$/ or die "misconfigured $_\n"; (lc $1) . "#$2"  } qw(
      quickstart/cpp/linux/from-microphone/helloworld.cpp#code
      quickstart/cpp/linux/text-to-speech/helloworld.cpp#code
      quickstart/cpp/macos/from-microphone/helloworld.cpp#code
      quickstart/cpp/macos/text-to-speech/helloworld.cpp#code
      quickstart/cpp/windows/from-file/helloworld/helloworld.cpp#code
      quickstart/cpp/windows/from-microphone/helloworld/helloworld.cpp#code
      quickstart/cpp/windows/intent-recognition/helloworld/helloworld.cpp#code
      quickstart/cpp/windows/text-to-speech/helloworld/helloworld.cpp#code
      quickstart/cpp/windows/translate-speech-to-text/helloworld/helloworld.cpp#code
      quickstart/csharp/dotnet/from-microphone/helloworld/Program.cs#code
      quickstart/csharp/dotnet/intent-recognition/helloworld/Program.cs#code
      quickstart/csharp/dotnet/text-to-speech/helloworld/Program.cs#code
      quickstart/csharp/dotnet/translate-speech-to-text/helloworld/Program.cs#code
      quickstart/csharp/dotnetcore/from-microphone/helloworld/Program.cs#code
      quickstart/csharp/dotnetcore/text-to-speech/helloworld/Program.cs#code
      quickstart/csharp/dotnetcore/translate-speech-to-text/helloworld/Program.cs#code
      quickstart/csharp/unity/from-microphone/Assets/Scripts/HelloWorld.cs#code
      quickstart/csharp/unity/text-to-speech/Assets/Scripts/HelloWorld.cs#code
      quickstart/csharp/uwp/from-microphone/helloworld/MainPage.xaml#StackPanel
      quickstart/csharp/uwp/from-microphone/helloworld/MainPage.xaml.cs#code
      quickstart/csharp/uwp/keyword-recognizer/helloworld/MainPage.xaml#StackPanel
      quickstart/csharp/uwp/keyword-recognizer/helloworld/MainPage.xaml.cs#code
      quickstart/csharp/uwp/text-to-speech/helloworld/MainPage.xaml#StackPanel
      quickstart/csharp/uwp/text-to-speech/helloworld/MainPage.xaml.cs#code
      quickstart/csharp/uwp/translate-speech-to-text/helloworld/MainPage.xaml#StackPanel
      quickstart/csharp/uwp/translate-speech-to-text/helloworld/MainPage.xaml.cs#code
      quickstart/java/android/from-microphone/app/src/main/java/com/microsoft/cognitiveservices/speech/samples/quickstart/MainActivity.java#code
      quickstart/java/android/keyword-recognizer/app/src/main/java/com/microsoft/cognitiveservices/speech/samples/quickstart/MainActivity.java#code
      quickstart/java/android/text-to-speech/app/src/main/java/com/microsoft/cognitiveservices/speech/samples/quickstart/MainActivity.java#code
      quickstart/java/android/text-to-speech/app/src/main/java/com/microsoft/cognitiveservices/speech/samples/quickstart/SpeakerStream.java#code
      quickstart/java/jre/from-microphone/pom.xml#dependencies
      quickstart/java/jre/from-microphone/pom.xml#repositories
      quickstart/java/jre/intent-recognition/pom.xml#dependencies
      quickstart/java/jre/intent-recognition/pom.xml#repositories
      quickstart/java/jre/from-microphone/src/speechsdk/quickstart/Main.java#code
      quickstart/java/jre/intent-recognition/src/speechsdk/quickstart/Main.java#code
      quickstart/java/jre/text-to-speech/pom.xml#dependencies
      quickstart/java/jre/text-to-speech/pom.xml#repositories
      quickstart/java/jre/text-to-speech/src/speechsdk/quickstart/Main.java#code
      quickstart/java/jre/translate-speech-to-text/src/speechsdk/quickstart/Main.java#code
      quickstart/java/jre/virtual-assistant/pom.xml#dependencies
      quickstart/java/jre/virtual-assistant/src/com/speechsdk/quickstart/ActivityAudioStream.java#code
      quickstart/java/jre/virtual-assistant/src/com/speechsdk/quickstart/Main.java#code
      quickstart/javascript/browser/from-microphone/index.html#authorizationfunction
      quickstart/javascript/browser/from-microphone/index.html#quickstartcode
      quickstart/javascript/browser/from-microphone/index.html#speechsdkref
      quickstart/javascript/browser/from-microphone/index.html#uidiv
      quickstart/javascript/browser/text-to-speech/index.html#authorizationfunction
      quickstart/javascript/browser/text-to-speech/index.html#quickstartcode
      quickstart/javascript/browser/text-to-speech/index.html#speechsdkref
      quickstart/javascript/browser/text-to-speech/index.html#uidiv
      quickstart/javascript/node/from-file/index.js#code
      quickstart/javascript/node/text-to-speech/index.js#code
      quickstart/objectivec/ios/from-microphone/helloworld/helloworld/ViewController.m#code
      quickstart/objectivec/ios/text-to-speech/helloworld/helloworld/AppDelegate.m#code
      quickstart/objectivec/ios/text-to-speech/helloworld/helloworld/ViewController.m#code
      quickstart/objectivec/macos/from-microphone/helloworld/helloworld/AppDelegate.m#code
      quickstart/objectivec/macos/text-to-speech/helloworld/helloworld/AppDelegate.m#code
      quickstart/python/from-microphone/quickstart.py#code
      quickstart/python/text-to-speech/quickstart.py#code
      quickstart/swift/ios/from-microphone/helloworld/helloworld/AppDelegate.swift#code
      quickstart/swift/ios/from-microphone/helloworld/helloworld/MicrosoftCognitiveServicesSpeech-Bridging-Header.h#code
      quickstart/swift/ios/from-microphone/helloworld/helloworld/ViewController.swift#code
      quickstart/swift/ios/text-to-speech/helloworld/helloworld/AppDelegate.swift#code
      quickstart/swift/ios/text-to-speech/helloworld/helloworld/MicrosoftCognitiveServicesSpeech-Bridging-Header.h#code
      quickstart/swift/ios/text-to-speech/helloworld/helloworld/ViewController.swift#code
      quickstart/swift/macos/from-microphone/helloworld/helloworld/AppDelegate.swift#code
      quickstart/swift/macos/from-microphone/helloworld/helloworld/MicrosoftCognitiveServicesSpeech-Bridging-Header.h#code
      samples/batch/csharp/program.cs#batchdefinition
      samples/batch/csharp/program.cs#batchstatus
      samples/cpp/windows/console/samples/intent_recognition_samples.cpp#IntentContinuousRecognitionWithFile
      samples/cpp/windows/console/samples/intent_recognition_samples.cpp#IntentRecognitionWithLanguage
      samples/cpp/windows/console/samples/intent_recognition_samples.cpp#IntentRecognitionWithMicrophone
      samples/cpp/windows/console/samples/intent_recognition_samples.cpp#toplevel
      samples/cpp/windows/console/samples/speech_recognition_samples.cpp#SpeechContinuousRecognitionWithFile
      samples/cpp/windows/console/samples/speech_recognition_samples.cpp#SpeechRecognitionUsingCustomizedModel
      samples/cpp/windows/console/samples/speech_recognition_samples.cpp#SpeechRecognitionWithMicrophone
      samples/cpp/windows/console/samples/speech_recognition_samples.cpp#toplevel
      samples/cpp/windows/console/samples/translation_samples.cpp#TranslationWithMicrophone
      samples/cpp/windows/console/samples/translation_samples.cpp#toplevel
      samples/csharp/sharedcontent/console/intent_recognition_samples.cs#intentContinuousRecognitionWithFile
      samples/csharp/sharedcontent/console/intent_recognition_samples.cs#intentRecognitionWithMicrophone
      samples/csharp/sharedcontent/console/intent_recognition_samples.cs#toplevel
      samples/csharp/sharedcontent/console/speech_recognition_samples.cs#recognitionContinuousWithFile
      samples/csharp/sharedcontent/console/speech_recognition_samples.cs#recognitionWithCompressedInputPullStreamAudio
      samples/csharp/sharedcontent/console/speech_recognition_samples.cs#recognitionWithCompressedInputPushStreamAudio
      samples/csharp/sharedcontent/console/speech_recognition_samples.cs#recognitionCustomized
      samples/csharp/sharedcontent/console/speech_recognition_samples.cs#recognitionWithMicrophone
      samples/csharp/sharedcontent/console/speech_recognition_samples.cs#toplevel
      samples/csharp/sharedcontent/console/translation_samples.cs#TranslationWithFileAsync
      samples/csharp/sharedcontent/console/translation_samples.cs#TranslationWithFileCompressedInputAsync
      samples/csharp/sharedcontent/console/translation_samples.cs#TranslationWithMicrophoneAsync
      samples/csharp/sharedcontent/console/translation_samples.cs#toplevel
      samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/IntentRecognitionSamples.java#IntentContinuousRecognitionWithFile
      samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/IntentRecognitionSamples.java#IntentRecognitionWithLanguage
      samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/IntentRecognitionSamples.java#IntentRecognitionWithMicrophone
      samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/IntentRecognitionSamples.java#toplevel
      samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/SpeechRecognitionSamples.java#recognitionContinuousWithFile
      samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/SpeechRecognitionSamples.java#recognitionCustomized
      samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/SpeechRecognitionSamples.java#recognitionWithMicrophone
      samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/SpeechRecognitionSamples.java#toplevel
      samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/TranslationSamples.java#TranslationWithFileAsync
      samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/TranslationSamples.java#TranslationWithMicrophoneAsync
      samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/TranslationSamples.java#toplevel
      samples/objective-c/ios/compressed-streams/CompressedStreamsSample/CompressedStreamsSample/ViewController.m#setup-stream
      samples/objective-c/ios/compressed-streams/CompressedStreamsSample/CompressedStreamsSample/ViewController.m#push-compressed-stream
      samples/python/console/intent_sample.py#IntentContinuousRecognitionWithFile
      samples/python/console/intent_sample.py#IntentRecognitionOnceWithFile
      samples/python/console/intent_sample.py#IntentRecognitionOnceWithMic
      samples/python/console/speech_sample.py#SpeechContinuousRecognitionWithFile
      samples/python/console/speech_sample.py#SpeechRecognitionUsingCustomizedModel
      samples/python/console/speech_sample.py#SpeechRecognitionWithFile
      samples/python/console/speech_sample.py#SpeechRecognitionWithMicrophone
      samples/python/console/translation_sample.py#TranslationContinuous
      samples/python/console/translation_sample.py#TranslationOnceWithFile
      samples/python/console/translation_sample.py#TranslationOnceWithMic
    );
  }

  our %expectedCount = map { $_ => 0 } @expectedTags;
}
sub ateof() {
  if (defined $m) {
    warn "DocFx codetags: tag $m not closed at end of file $oldargv.\n";
    $err++
  }
}
if ($ARGV ne $oldargv) {
  $. = 1;
  ateof();
  $oldargv = $ARGV;
  $m = undef;

  if ($ARGV =~ /\.xa?ml$|\.html$/) {
    $s = $xml_s;
    $e = $xml_e;
  } elsif ($ARGV =~ /\.py$/) {
    $s = $py_s;
    $e = $py_e;
  } else {
    $s = $c_s;
    $e = $c_e;
  }
}
if (defined $m) {
  if (m/$s/) {
    warn "DocFx codetags: nested tag $1 not supported in file $ARGV, line $.\n";
    $m = $1;
    $err++
  }
  if (m/$e/) {
    if ($1 ne $m) {
      warn "DocFx codetags: Unmatched tag $1 in file $ARGV, line $.\n";
      $err++
    }
    $m = undef;
  }
} else {
  if (m/$s/) {
    $m = $1;

    my $id = (lc $ARGV). "#$1";
    if (exists $expectedCount{$id}) {
      if ($expectedCount{$id} != 0) {
        warn "DocFx codetags: duplicate tag $1 in file $ARGV, line $.\n";
        $err++;
      }
      $expectedCount{$id}++;
    } else {
      warn "DocFx codetags: unexpected tag $1 in file $ARGV, line $.; contact wolfma/zhouwang/mahilleb\n";
      $err++;
    }
  }
  if (m/$e/) {
    warn "DocFx codetags: Unexpected end tag $1 in file $ARGV, line $.\n";
    $err++
  }
}

END {
  ateof();

  # Check links that haven't been found
  for my $id (keys %expectedCount) {
    if ($expectedCount{$id} == 0) {
      warn "DocFx codetags: tag $id was expected, but not found; contact wolfma/zhouwang/mahilleb\n";
    }
  }

  # Verify mandatory files are present
  for my $file (@expectedFiles) {
    -r "$file" or do {
      warn "DocFx codetags: file $file must exist, but not found; contact wolfma/zhouwang/mahilleb\n";
      $err++;
    }
  }

  exit 1 if $err > 0;
}
