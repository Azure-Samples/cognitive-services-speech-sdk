#!/usr/bin/env bash
# Run in an empty ubuntu:16.04 docker

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

set -u -x -e -o pipefail

# Note: add lib32z1-dev libc6-dev-i386 linux-libc-dev to PACKAGES if doing linux-x86 cross compile.
PACKAGES="build-essential ca-certificates perl unzip wget make"

if dpkg -s $PACKAGES 1>/dev/null 2>/dev/null; then
  printf "Packages already installed, skipping.\n"
else
  export DEBIAN_FRONTEND=noninteractive
  apt-get update
  apt-get install --yes --quiet --no-install-recommends $PACKAGES
  apt-get autoremove --yes --quiet
fi

OPENSSL_VERSION=1.1.1b
OPENSSL_BASE=openssl-$OPENSSL_VERSION
OPENSSL_FILE=$OPENSSL_BASE.tar.gz
OPENSSL_SHA256SUM=5c557b023230413dfb0756f3137a13e6d726838ccd1430888ad15bfb2b43ea4b

BASE_INSTALL_DIR=$PWD/external/linux-openssl
# Clean all outputs
echo cleaning outputs from $BASE_INSTALL_DIR
rm -rf "$BASE_INSTALL_DIR"

[[ -f $OPENSSL_FILE ]] ||
  wget --no-verbose "https://www.openssl.org/source/$OPENSSL_FILE"
echo "$OPENSSL_SHA256SUM  $OPENSSL_FILE" | sha256sum --check --strict -
tar -xzf "$OPENSSL_FILE"
cd "$OPENSSL_BASE"
export LD_LIBRARY_PATH="$OPENSSL_BASE"/lib

if [[ $# == 0 ]]; then
  # Default targets (all)
  targets=(linux-x86_64 linux-x86 linux-aarch64 linux-armv4)
else
  # Specific targets from command line
  targets=("$@")
fi

OPTIONS=()

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
    -fvisibility=hidden
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

for SSL_TARGET in "${targets[@]}"; do
  echo Building for target $SSL_TARGET.

  # Ignore errors here
  make clean || true

  INSTALL_DIR=$BASE_INSTALL_DIR/$SSL_TARGET

  ./Configure "$SSL_TARGET" --openssldir=/usr/lib/ssl \
    "${OPTIONS[@]}" \
    zlib \
    no-asm \
    no-tests \
    no-shared \
    no-unit-test

    make -j $(nproc)
    make install DESTDIR=$INSTALL_DIR
    # Informational:
    ./configdata.pm --dump
done