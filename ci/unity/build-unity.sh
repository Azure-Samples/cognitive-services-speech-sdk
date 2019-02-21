#!/bin/bash

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

# Get some helpers
. "$SCRIPT_DIR/../functions.sh"

set -u -x -e -o pipefail
USAGE="Usage: $0 drop-dir"
DROP_DIR="${1?$USAGE}"

# Make absolute
DROP_DIR="$(readlink -f "$DROP_DIR")"

# Used for timestamping all published files
# (Likely ineffective when building the Unity package)
NOW=$(date -Iseconds)

for flavor in Release; do # no Debug for now

  UNITY_DIR="$SCRIPT_DIR/unity"

  [[ $flavor == Debug ]] && UNITY_DIR+=-debug

  # Clean output
  [[ -d $UNITY_DIR ]] && rm -rf "$UNITY_DIR"

  # Copy unity sample code, which is required in order to -exportPackage command would work
  cp -r "$SCRIPT_DIR/../../public_samples/quickstart/csharp-unity" "$UNITY_DIR"

  # Copy required meta information from template (e.g., library settings)
  cp -r "$SCRIPT_DIR/SpeechSDK-template" "$UNITY_DIR/Assets/SpeechSDK"

  # Copy libraries
  cp --verbose --preserve \
    "$DROP_DIR"/Android/Android-arm32/$flavor/public/lib/libMicrosoft.CognitiveServices.Speech.core.so \
    "$UNITY_DIR"/Assets/SpeechSDK/Plugins/Android/libs/armeabi-v7a

  cp --verbose --preserve \
    "$DROP_DIR"/Android/Android-arm64/$flavor/public/lib/libMicrosoft.CognitiveServices.Speech.core.so \
    "$UNITY_DIR"/Assets/SpeechSDK/Plugins/Android/libs/arm64-v8a

  cp --verbose --preserve \
    "$DROP_DIR"/Android/Android-x86/$flavor/public/lib/libMicrosoft.CognitiveServices.Speech.core.so \
    "$UNITY_DIR"/Assets/SpeechSDK/Plugins/Android/libs/x86

  cp --verbose --preserve \
    "$DROP_DIR"/WindowsUwp/Win32/$flavor/public/lib/Microsoft.CognitiveServices.Speech.core.dll \
    "$UNITY_DIR"/Assets/SpeechSDK/Plugins/WSA/x86

  cp --verbose --preserve \
    "$DROP_DIR"/WindowsUwp/x64/$flavor/public/lib/Microsoft.CognitiveServices.Speech.core.dll \
    "$UNITY_DIR"/Assets/SpeechSDK/Plugins/WSA/x64

  cp --verbose --preserve \
    "$DROP_DIR"/WindowsUwp/ARM/$flavor/public/lib/Microsoft.CognitiveServices.Speech.core.dll \
    "$UNITY_DIR"/Assets/SpeechSDK/Plugins/WSA/ARM

  cp --verbose --preserve \
    "$DROP_DIR"/Windows/Win32/$flavor/public/lib/Microsoft.CognitiveServices.Speech.core.dll \
    "$UNITY_DIR"/Assets/SpeechSDK/Plugins/x86

  cp --verbose --preserve \
    "$DROP_DIR"/Windows/x64/$flavor/public/lib/Microsoft.CognitiveServices.Speech.core.dll \
    "$UNITY_DIR"/Assets/SpeechSDK/Plugins/x86_64

  cp --verbose --preserve \
    "$DROP_DIR"/Windows/x64/$flavor/public/lib/net461/Microsoft.CognitiveServices.Speech.csharp.dll \
    "$UNITY_DIR"/Assets/SpeechSDK/Plugins/x86_64

  cp --verbose --preserve \
    "$DROP_DIR"/Windows/x64/$flavor/public/lib/UnixOS/netstandard2.0/Microsoft.CognitiveServices.Speech.csharp.dll \
    "$UNITY_DIR"/Assets/SpeechSDK/Plugins/x86_64/Microsoft.CognitiveServices.Speech.csharp.unix.dll

  cp --verbose "$SCRIPT_DIR/../../"{license.md,ThirdPartyNotices.md,REDIST.txt} "$UNITY_DIR"/Assets/SpeechSDK

  # Timestamp
  find "$UNITY_DIR" -print0 | xargs -0 touch --date=$NOW

done
