#!/bin/bash

VERSIONRE="${1-.*}"

declare -A redirectRe
redirectRe=(
  # TODO some aren't yet checked
  # Main entry page
  ["csspeech"]=".*"
  # API Reference for C++
  ["csspeech/cppref"]=".*/cpp/annotated\.html"
  # Reference for C++ Recognizer class
  ["csspeech/cppref/recognizer"]=".*/cpp/.*recognizer\.html"
  # Reference for C++ RecognizerFactory class
  ["csspeech/cppref/recognizerfactory"]=".*/cpp/.*recognizer_factory\.html"
  # API Reference for C#
  ["csspeech/csharpref"]=".*/csharp/annotated\.html"
  # Reference for C# Recognizer class
  ["csspeech/csharpref/recognizer"]=".*/csharp/.*recognizer\.html"
  # Reference for C# RecognizerFactory class
  ["csspeech/csharpref/recognizerfactory"]=".*/csharp/.*recognizer_factory\.html"
  # API Reference for Java
  ["csspeech/javaref"]=".*/java/annotated\.html"
  # Reference for Java Recognizer class
  ["csspeech/javaref/recognizer"]=".*/java/.*recognizer\.html"
  # Reference for Java RecognizerFactory class
  ["csspeech/javaref/recognizerfactory"]=".*/java/.*recognizer_factory\.html"
  # License for (latest) binaries
  ["csspeech/license"]=".*/license\.html"
  # Archive (.tar.gz) for latest Linux binaries
  ["csspeech/linuxbinary"]=".*/csspeech-$VERSIONRE\.tar\.gz"
  # Archive (.tar.gz) for latest Linux samples and tutorials
  ["csspeech/linuxsample"]=".*/SpeechSDK-LinuxSamples\.tar\.gz"
  # Archive (.tar.gz) for latest Linux symbols matching the latest Linux binaries
  ["csspeech/linuxsymbols"]=".*TODO.*"
  # Landing page for package on nuget.org
  ["csspeech/nuget"]="https://www.nuget.org/packages/Microsoft\.CognitiveServices\.Speech"
  # Archive (.zip) for latest Windows binaries
  ["csspeech/winbinary"]=".*TODO.*"
  # Archive (.zip) for latest Windows samples and tutorials
  ["csspeech/winsample"]=".*/SpeechSDK-WindowsSamples-$VERSIONRE\.zip"
  # Archive (.zip) for Windows debug symbols matching the latest Windows binaries
  ["csspeech/winsymbols"]=".*TODO.*"

  # TODO later - https://csspeechstorage.blob.core.windows.net/
)

errors=0
for shortLink in "${!redirectRe[@]}"; do
  re="${redirectRe[$shortLink]}"
  link="https://aka.ms/$shortLink"
  redirect="$(curl -s -I "https://aka.ms/$shortLink" | grep ^Location: | cut -d' ' -f2)"

  stat=n/a

  if [[ $? != 0 ]]; then
    stat=ERROR
  elif [[ -z $redirect ]]; then
    stat=EMPTY
  elif [[ $redirect == "http://www.microsoft.com" ]]; then
    stat=UNSET
  elif [[ $redirect =~ ^$re$ ]]; then
    stat=OK
  else
    stat=WRONG
  fi
  [[ $stat == OK ]] || ((errors++))
  printf "[%-8s] %s -> %s\n" "$stat" "$link" "$redirect"

  # TODO also check target
done

if ((errors != 0)); then
  echo $errors occurred.
  exit 1
else
  echo Everything is fine.
fi
