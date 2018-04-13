#!/bin/bash

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
SOURCE_ROOT="$SCRIPT_DIR/.."
C_API_DIR="$SOURCE_ROOT/source/public/c_api"

gcc -x c -I "$C_API_DIR" - <<CODE
#include <wchar.h>
#include <stdint.h>
#include <speechapi_c.h>
int main() { return 0; }
CODE
