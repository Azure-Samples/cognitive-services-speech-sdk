#!/bin/bash
#
# Copyright 2016 leenjewel
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -u

source ./_shared.sh

# Setup architectures, library name and other vars + cleanup from previous runs
LIB_NAME=${TOOLS_ROOT}/../external/curl
LIB_DEST_DIR=${TOOLS_ROOT}/libs

echo $LIB_NAME
echo $LIB_DEST_DIR

[ -d ${LIB_DEST_DIR} ] && rm -rf ${LIB_DEST_DIR}
[ -d ${LIB_NAME} ] || "could not find openssl in externals folder";


# Unarchive library, then configure and make for specified architectures
configure_make() {
  ARCH=$1; ABI=$2;
  pushd "${LIB_NAME}"

  configure $*

  # fix me (currently depends on build-openssl-android.sh being executed first!)
  cp ${TOOLS_ROOT}/../output/android/openssl-${ABI}/lib/libssl.a ${SYSROOT}/usr/lib
  cp ${TOOLS_ROOT}/../output/android/openssl-${ABI}/lib/libcrypto.a ${SYSROOT}/usr/lib
  cp -r ${TOOLS_ROOT}/../output/android/openssl-${ABI}/include/openssl ${SYSROOT}/usr/include

  mkdir -p ${LIB_DEST_DIR}/${ABI}
  ./configure --prefix=${LIB_DEST_DIR}/${ABI} \
              --with-sysroot=${SYSROOT} \
              --host=arm-linux-androideabi \
              --with-ssl=${SYSROOT}/usr \
              --enable-ipv6 \
              --enable-static \
              --enable-threaded-resolver \
              --disable-dict \
              --disable-gopher \
              --disable-ldap --disable-ldaps \
              --disable-manual \
              --disable-pop3 --disable-smtp --disable-imap \
              --disable-rtsp \
              --disable-shared \
              --disable-smb \
              --disable-telnet \
              --disable-verbose \
              --without-librtmp
  PATH=$TOOLCHAIN_PATH:$PATH

  make clean

  if make -j4
  then
    make install

    OUTPUT_ROOT=${TOOLS_ROOT}/../output/android/curl-${ABI}
    [ -d ${OUTPUT_ROOT}/include ] || mkdir -p ${OUTPUT_ROOT}/include
    cp -r ${LIB_DEST_DIR}/${ABI}/include/curl ${OUTPUT_ROOT}/include

    [ -d ${OUTPUT_ROOT}/lib ] || mkdir -p ${OUTPUT_ROOT}/lib
    cp ${LIB_DEST_DIR}/${ABI}/lib/libcurl.a ${OUTPUT_ROOT}/lib
  fi;
  popd;
}

for ((i=0; i < ${#ARCHS[@]}; i++))
do
  if [[ $# -eq 0 ]] || [[ "$1" == "${ARCHS[i]}" ]]; then
    [[ ${ANDROID_API} < 21 ]] && ( echo "${ABIS[i]}" | grep 64 > /dev/null ) && continue;
    configure_make "${ARCHS[i]}" "${ABIS[i]}"
  fi
done
