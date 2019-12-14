#!/usr/bin/env bash
# Run in an empty ubuntu:16.04 docker

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

set -u -x -e -o pipefail

BASE_INSTALL_DIR=$PWD/android-openssl

# Clean all outputs
rm -rf "$BASE_INSTALL_DIR"

PACKAGES="ca-certificates perl unzip wget make"

if dpkg -s $PACKAGES 1>/dev/null 2>/dev/null; then
  printf "Packages already installed, skipping.\n"
else
  export DEBIAN_FRONTEND=noninteractive
  apt-get update || true
  apt-get dist-upgrade --yes --quiet
  apt-get install --yes --quiet --no-install-recommends $PACKAGES
  apt-get autoremove --yes --quiet
fi

NDK_VERSION=r16b
NDK_DIR=android-ndk-$NDK_VERSION
NDK_PLATFORM=linux-x86_64
NDK_FILE=$NDK_DIR-$NDK_PLATFORM.zip
NDK_SHA256SUM=bcdea4f5353773b2ffa85b5a9a2ae35544ce88ec5b507301d8cf6a76b765d901

[[ -f $NDK_FILE ]] ||
  wget --no-verbose "https://dl.google.com/android/repository/$NDK_FILE"

echo "$NDK_SHA256SUM  $NDK_FILE" | sha256sum --check --strict -

[[ -d $NDK_DIR ]] || {
  unzip -q $NDK_FILE
  [[ -d $NDK_DIR ]]
}
export ANDROID_NDK=$PWD/$NDK_DIR

OPENSSL_VERSION=1.1.1b
OPENSSL_BASE=openssl-$OPENSSL_VERSION
OPENSSL_FILE=$OPENSSL_BASE.tar.gz
OPENSSL_SHA256SUM=5c557b023230413dfb0756f3137a13e6d726838ccd1430888ad15bfb2b43ea4b

[[ -f $OPENSSL_FILE ]] ||
  wget --no-verbose "https://www.openssl.org/source/$OPENSSL_FILE"
echo "$OPENSSL_SHA256SUM  $OPENSSL_FILE" | sha256sum --check --strict -
tar -xzf "$OPENSSL_FILE"
cd "$OPENSSL_BASE"

if [[ $# == 0 ]]; then
  # Default targets (all)
  targets=(armeabi-v7a arm64-v8a x86 x86_64)
else
  # Specific targets from command line
  targets=("$@")
fi

originalPath=$PATH

for target in "${targets[@]}"; do
  echo Building for target $target.

  INSTALL_DIR=$BASE_INSTALL_DIR/$target

  # Note: API_LEVEL match what we current have in the main Speech SDK build.

  case $target in
    armeabi-v7a)
      TRIPLE=arm-linux-androideabi
      TC_NAME=arm-linux-androideabi-4.9
      OPTIONS=(--target=armv7a-linux-androideabi -Wl,--fix-cortex-a8)
      SSL_TARGET=android-arm
      API_LEVEL=19
      ;;
    x86)
      TRIPLE=i686-linux-android
      TC_NAME=x86-4.9
      OPTIONS=()
      SSL_TARGET=android-x86
      API_LEVEL=19
      ;;
    x86_64)
      TRIPLE=x86_64-linux-android
      TC_NAME=x86_64-4.9
      OPTIONS=()
      SSL_TARGET=android-x86_64
      API_LEVEL=21
      ;;
    arm64-v8a)
      TRIPLE=aarch64-linux-android
      TC_NAME=aarch64-linux-android-4.9
      OPTIONS=()
      SSL_TARGET=android-arm64
      API_LEVEL=21
      ;;
  esac
  OPTIONS+=(-D__ANDROID_API__="$API_LEVEL")

  # Additional compile options.
  # Note: some do not apply as we're building a static library.
  OPTIONS+=(
    # SafeStack
    # -fsanitize=safe-stack # core dumps in NDK r16b clang
    # Stack Protection
    -fstack-protector-strong
    # (-fstack-protector-all is also ok, but how does it impact perf?)
    # Control Flow Integrity (link errors building OpenSSL for ARMv8; also link errors in the Speech SDK)
    # -flto -fsanitize=cfi
    # -fvisibility=hidden
    # Address Space Layout Randomization
    -fPIE -pie # for executables
    -fPIC # for shared libraries
    # GOT Protection
    -Wl,-z,relro -Wl,-z,now
    # Format String Warnings
    -Wformat -Wformat-security
    -Werror=format-security
    # Debug symbols
    -g
    # Drop-in replacement for banned API usage. Probably not required for
    # OpenSSL, but should not hurt.
    -D_FORTIFY_SOURCE=2
  )

  TOOLCHAIN=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64
  export PATH=$TOOLCHAIN/$TRIPLE/bin:$TOOLCHAIN/bin:$originalPath

  # Ignore errors here
  make clean || true

  # (Note: NDK 19 would neeed -fuse-ld="$TOOLCHAIN/$TRIPLE/bin/ld" as well)

  ./Configure "$SSL_TARGET" \
    "${OPTIONS[@]}" \
    zlib \
    no-asm \
    no-tests \
    no-shared \
    no-unit-test

  make -j $(nproc)
  make install_sw DESTDIR=$INSTALL_DIR
  # Informational:
  ./configdata.pm --dump
done
