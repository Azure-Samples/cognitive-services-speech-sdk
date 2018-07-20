#!/bin/bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
# To be sourced from depending scripts.

# https://github.com/Microsoft/vsts-tasks/blob/master/docs/authoring/commands.md

# Ensure now logging of commands to not confuse the agent...

vsts_setvar() (
  set +x
  echo Setting Build Variable $1=$2
  echo "##vso[task.setvariable variable=$1]$2"
)

vsts_setvars_by_ref() (
  set +x
  for i in "$@"; do
    vsts_setvar "$i" "${!i}"
  done
)

vsts_setoutvar() (
  set +x
  echo Setting Build Output Variable $1=$2
  echo "##vso[task.setvariable variable=$1;isOutput=true]$2"
)

vsts_updatebuildnumber() (
  set +x
  echo Updating build number to $1
  echo "##vso[build.updatebuildnumber]$1"
)

vsts_addbuildtag() (
  set +x
  echo Adding build tag $1
  echo "##vso[build.addbuildtag]$1"
)

function existsExactlyOneDir {
  [[ $# -eq 1 && -d $1 ]]
}

function existsExactlyOneFile {
  [[ $# -eq 1 && -f $1 ]]
}

# Print arguments, surrounded by lines of equal characters.
function pretty_print {
  printf '%.0s=' {1..78}
  echo
  printf '%s\n' "$@"
  printf '%.0s=' {1..78}
  echo
}

# Print variables and their values, and separators (- or =)
function print_vars {
  for i in "$@"; do
    case $i in
      [=-])
         printf "%.0s$i" {1..78}
         echo
         ;;
      *) printf "%-30s  %-30s\n" "$i" "${!i}" ;;
    esac
  done
}

function joinArgs {
  SEP=$1
  shift 1

  [[ $# -ge 1 ]] && {
    printf '%s' $1
    shift 1
  }
  [[ $# -ge 1 ]] && printf "$SEP%s" "$@"
}

function die {
    set +x
    printf "$1"
    exit 1
}
