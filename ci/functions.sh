#!/bin/bash
# To be sourced from depending scripts.

# TODO add variant for "isOutput=true"? or always?
function vsts_setvar {
  echo Setting Build Variable $1=$2
  echo "##vso[task.setvariable variable=$1]$2"
}

function vsts_updatebuildnumber {
  echo Updating build number to $1
  echo "##vso[build.updatebuildnumber]$1"
}

function existsExactlyOneDir {
  [[ $# -eq 1 && -d $1 ]]
}

function existsExactlyOneFile {
  [[ $# -eq 1 && -f $1 ]]
}
