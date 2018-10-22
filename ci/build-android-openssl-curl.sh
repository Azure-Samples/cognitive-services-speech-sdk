#!/bin/bash
# Run in a empty ubuntu:16.04 docker

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

set -x -e -o pipefail

[[ -d speechsdkext ]] || mkdir speechsdkext
cd speechsdkext

PACKAGES="ca-certificates git perl python unzip wget"

if dpkg -s $PACKAGES 1>/dev/null 2>/dev/null; then
  printf "Packages already installed, skipping.\n"
else
  export DEBIAN_FRONTEND=noninteractive
  apt-get update
  apt-get dist-upgrade --yes --quiet
  apt-get install --yes --quiet --no-install-recommends $PACKAGES
  apt-get autoremove --yes --quiet
fi

NDK_VERSION=r14b
NDK_DIR=android-ndk-$NDK_VERSION
NDK_PLATFORM=linux-x86_64
NDK_FILE=$NDK_DIR-$NDK_PLATFORM.zip
NDK_SHA256SUM=0ecc2017802924cf81fffc0f51d342e3e69de6343da892ac9fa1cd79bc106024

if [[ ! -f "$NDK_FILE" ]]; then
  NDK_URL="https://dl.google.com/android/repository/$NDK_FILE"
  wget "$NDK_URL"
fi

echo "$NDK_SHA256SUM  $NDK_FILE" | sha256sum --check --strict -

[[ -d $NDK_DIR ]] || {
  unzip $NDK_FILE
  [[ -d $NDK_DIR ]]
}
export ANDROID_NDK=$PWD/$NDK_DIR
[[ -d openssl_for_ios_and_android ]] || {
  git clone https://github.com/leenjewel/openssl_for_ios_and_android
  [[ -d openssl_for_ios_and_android ]]
}

cd openssl_for_ios_and_android
git checkout 11b90af382cd43b540df5d66a0b0fc9c572442d9
git reset --hard HEAD
git clean -fdx
export ANDROID_API=21
cd tools
ARCHS="android-armeabi android64-aarch64 android-x86 android64"
perl -pi - $ARCHS <<"SCRIPT"
BEGIN {
  our %arch2abi = qw/
    android armeabi
    android-armeabi armeabi-v7a
    android64-aarch64 arm64-v8a
    android-x86 x86
    android64 x86_64
    android-mips mips
    android-mips64 mips64
  /;
  our @abi = map {
    exists $arch2abi{$_} or die "Unknown architecture $_.\n";
    $arch2abi{$_}
  } @ARGV;
  our @arch = splice @ARGV;
  @ARGV = qw/_shared.sh/;
}
s/^ARCHS=.*/"ARCHS=(" . (join ' ', map { qq("$_") } @arch). ")"/e;
s/^ABIS=.*/"ABIS=(" . (join ' ', map { qq("$_") } @abi). ")"/e;
SCRIPT
perl -i -pe 's:^(source ./_shared.sh)$:$1 "":; s/LIB_NAME="openssl-.*"/LIB_NAME="openssl-1.1.0i"/' build-openssl4android.sh
perl -i -pe 's/LIB_NAME="curl-.*"/LIB_NAME="curl-7.61.1"/; s:--with-ssl=/usr:--with-ssl=\${SYSROOT}/usr --disable-dependency-tracking --without-librtmp --disable-FILE --disable-FTP --disable-FTPS --disable-HTTP --disable-TFTP:' build-curl4android.sh
git --no-pager diff
./build-openssl4android.sh || {
  echo Failed building openssl.
  exit 1
}
./build-curl4android.sh || {
  echo Failed building curl.
  exit 1
}
