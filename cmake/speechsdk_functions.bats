#!/usr/bin/env bats

speechsdkFunctions="$BATS_TEST_DIRNAME/speechsdk_functions.cmake"

function printargs() {
  local prefix="$1"
  prefix="$1"
  shift
  printf "# $prefix: %s\n" "$@" >&3
}

function printargs_false() {
  printargs "$@"
  false
}

export TEST_CMAKE_SCRIPT="$BATS_TMPDIR/1.cmake"
trap 'rm "$TEST_CMAKE_SCRIPT" || true' EXIT

function parse_version_test() {
  cat > "$TEST_CMAKE_SCRIPT" <<SCRIPT
include("$speechsdkFunctions")
parse_version(BUILD_TYPE "$1" VERSION "$2" PREFIX something)
SCRIPT
  run cmake -P "$TEST_CMAKE_SCRIPT"
}

function test_parse_version() {
  local ver expectSuccess="$1" buildType="$2"
  shift 2
  for ver in "$@"; do
    parse_version_test "$buildType" $ver
    if [[ $expectSuccess == valid ]]; then
      [[ $status = 0 ]] || printargs_false cmake "${lines[@]}"
    else
      [[ $status != 0 ]] || printargs_false cmake "exited with success for version $ver!" "${lines[@]}"
    fi
  done
}

@test "cmake is present" {
  type cmake
}

@test "cmake can source functions present" {
  cmake -P "$speechsdkFunctions"
}

validLocal=({1.1.0,1.0.1}-alpha.0.1)
validDev=({1.1.0,1.0.1}-alpha.0.42)
validInt=({1.1.0,1.0.1}-beta.0.42)
validProd=(1.2.3 1.1.0 1.0.2-{alpha,beta,rc}.{1,7})
nonParsing=(0 1{,.{,2{,.{,3-{,blah,beta{,.{,42.{,47.{,11}}}}}}}}})

@test "valid version numbers for local build are accepted" {
  test_parse_version valid local "${validLocal[@]}"
}

@test "valid version numbers for dev build are accepted" {
  test_parse_version valid dev "${validDev[@]}"
}

@test "valid version numbers for int build are accepted" {
  test_parse_version valid int "${validInt[@]}"
}

@test "valid version numbers for prod build are accepted" {
  test_parse_version valid prod "${validProd[@]}"
}

@test "invalid version numbers for local build are rejected" {
  test_parse_version invalid local "${nonParsing[@]}" "${validDev[@]}" "${validInt[@]}" "${validProd[@]}"
}

@test "invalid version numbers for dev build are rejected" {
  # N.B. validLocal also valid for dev
  test_parse_version invalid dev "${nonParsing[@]}" "${validProd[@]}"
}

@test "invalid version numbers for int build are rejected" {
  test_parse_version invalid int "${nonParsing[@]}" "${validLocal[@]}" "${validDev[@]}" "${validProd[@]}"
}

@test "invalid version numbers for prod build are rejected" {
  test_parse_version invalid prod "${nonParsing[@]}" "${validLocal[@]}" "${validDev[@]}" "${validInt[@]}"
}
