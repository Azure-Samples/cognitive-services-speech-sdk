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
LIB_NAME=${TOOLS_ROOT}/../external/openssl
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

  #support openssl-1.0.x
  #if [[ $LIB_NAME != openssl-1.1.* ]]; then
  #  if [[ $ARCH == "android-armeabi" ]]; then
  #      ARCH="android-armv7"
  #  elif [[ $ARCH == "android64" ]]; then 
  #      ARCH="linux-x86_64 shared no-ssl2 no-ssl3 no-hw "
  #  elif [[ "$ARCH" == "android64-aarch64" ]]; then
  #      ARCH="android shared no-ssl2 no-ssl3 no-hw "
  #  fi
  #fi

  ./Configure $ARCH \
              --prefix=${LIB_DEST_DIR}/${ABI} \
              --with-zlib-include=$SYSROOT/usr/include \
              --with-zlib-lib=$SYSROOT/usr/lib \
              zlib \
              no-asm \
              no-shared \
              no-unit-test \
              no-stdio
  PATH=$TOOLCHAIN_PATH:$PATH

  make clean
  
  if make -j4; then
    # make install

    make -n install_sw
    make -n install_ssldirs

    make install_sw
    make install_ssldirs

    OUTPUT_ROOT=${TOOLS_ROOT}/../output/android/openssl-${ABI}
    [ -d ${OUTPUT_ROOT}/include ] || mkdir -p ${OUTPUT_ROOT}/include
    cp -r ${LIB_DEST_DIR}/${ABI}/include/openssl ${OUTPUT_ROOT}/include

    [ -d ${OUTPUT_ROOT}/lib ] || mkdir -p ${OUTPUT_ROOT}/lib
    cp ${LIB_DEST_DIR}/${ABI}/lib/libcrypto.a ${OUTPUT_ROOT}/lib
    cp ${LIB_DEST_DIR}/${ABI}/lib/libssl.a ${OUTPUT_ROOT}/lib
  fi;
  popd

}

for ((i=0; i < ${#ARCHS[@]}; i++))
do
  if [[ $# -eq 0 ]] || [[ "$1" == "${ARCHS[i]}" ]]; then
    # Do not build 64 bit arch if ANDROID_API is less than 21 which is
    # the minimum supported API level for 64 bit.
    [[ ${ANDROID_API} < 21 ]] && ( echo "${ABIS[i]}" | grep 64 > /dev/null ) && continue;
    configure_make "${ARCHS[i]}" "${ABIS[i]}"
  fi
done
