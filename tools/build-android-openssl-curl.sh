#!/bin/bash

# Run in a empty ubuntu:16.04 docker
mkdir speechsdkext
cd speechsdkext
apt-get update
apt-get dist-ugprade --yes
apt-get install --yes git wget perl python unzip 
apt-get autoremove --yes
wget https://dl.google.com/android/repository/android-ndk-r14b-linux-x86_64.zip
unzip android-ndk-r14b-linux-x86_64.zip
rm android-ndk-r14b-linux-x86_64.zip
export ANDROID_NDK=$PWD/android-ndk-r14b
git clone https://github.com/leenjewel/openssl_for_ios_and_android
cd openssl_for_ios_and_android
git checkout 11b90af382cd43b540df5d66a0b0fc9c572442d9
export ANDROID_API=21
cd tools
perl -i -pe 's/^ARCHS=.*/ARCHS=("android-armeabi" "android64-aarch64")/; s/^ABIS=.*/ABIS=("armeabi-v7a" "arm64-v8a")/' _shared.sh
perl -i -pe 's:^(source ./_shared.sh)$:$1 "":; s/LIB_NAME="openssl-.*"/LIB_NAME="openssl-1.1.0i"/' build-openssl4android.sh
perl -i -pe 's/LIB_NAME="curl-.*"/LIB_NAME="curl-7.61.1"/; s:--with-ssl=/usr:--with-ssl=\${SYSROOT}/usr --disable-dependency-tracking --without-librtmp --disable-FILE --disable-FTP --disable-FTPS --disable-HTTP --disable-TFTP:' build-curl4android.sh
git diff
./build-openssl4android.sh
./build-curl4android.sh
