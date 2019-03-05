#!/usr/bin/env bash

VERSIONRE="${1-.*}"

declare -A redirectRe
redirectRe=(
  # TODO on docs, we shouldn't link to specific language (en-us)

  # TODO some aren't yet checked / exposed

  # Main entry page
  ["csspeech"]="https://review\.docs\.microsoft\.com/azure/cognitive-services/speech-service/sdk/overview\?branch=pr-en-us-36555"

  # API Reference for C++
  ["csspeech/cppref"]="https://review\.docs\.microsoft\.com/cpp/cognitive-services/speech/index\?branch=master"
  # Reference for C++ Recognizer class
  ["csspeech/cppref/recognizer"]="https://review\.docs\.microsoft\.com/cpp/cognitive-services/speech/recognizer\?branch=master"
  # Reference for C++ RecognizerFactory class
  ["csspeech/cppref/recognizerfactory"]="https://review\.docs\.microsoft\.com/cpp/cognitive-services/speech/speechfactory\?branch=master"
  # API Reference for C#
  ["csspeech/csharpref"]="https://review\.docs\.microsoft\.com/dotnet/api/microsoft\.cognitiveservices\.speech\?view=azure-dotnet&branch=pr-en-us-607"
  # Reference for C# Recognizer class
  ["csspeech/csharpref/recognizer"]="https://review\.docs\.microsoft\.com/dotnet/api/microsoft\.cognitiveservices\.speech\.recognizer\?view=azure-dotnet&branch=pr-en-us-607"
  # Reference for C# RecognizerFactory class
  ["csspeech/csharpref/recognizerfactory"]="https://review\.docs\.microsoft\.com/dotnet/api/microsoft\.cognitiveservices\.speech\.speechfactory\?view=azure-dotnet&branch=pr-en-us-607"
  # TODO Java should point to Mikki's preview
  # API Reference for Java
  ["csspeech/javaref"]=".*/java/annotated\.html"
  # Reference for Java Recognizer class
  ["csspeech/javaref/recognizer"]=".*/java/.*recognizer\.html"
  # Reference for Java RecognizerFactory class
  ["csspeech/javaref/recognizerfactory"]=".*/java/.*recognizer_factory\.html"
  # License for (latest) binaries
  ["csspeech/license"]="https://review\.docs\.microsoft\.com/azure/cognitive-services/speech-service/sdk/license\?branch=pr-en-us-36555"
  # Archive (.tar.gz) for latest Linux binaries
  ["csspeech/linuxbinary"]=".*/csspeech-$VERSIONRE\.tar\.gz"
  # Archive (.tar.gz) for latest Linux samples and tutorials
  ["csspeech/linuxsample"]=".*/SpeechSDK-LinuxSamples-$VERSIONRE\.tar\.gz"
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
  # Privacy statement
  ["csspeech/privacy"]="https://go.microsoft.com/fwlink/\?LinkId=521839"
  # Language Understanding documentation
  ["csspeech/luisdocs"]="https://review\.docs\.microsoft\.com/azure/cognitive-services/luis/\?branch=release-build-luis"

  # TODO later - https://csspeechstorage.blob.core.windows.net/
)

errors=0
for shortLink in $(printf "%s\n" "${!redirectRe[@]}" | sort); do
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

  # TODO also check target [currently not all are accessible without login]
done

if ((errors != 0)); then
  echo $errors occurred.
  exit 1
else
  echo Everything is fine.
fi
