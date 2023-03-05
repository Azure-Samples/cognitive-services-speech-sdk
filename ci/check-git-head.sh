#!/usr/bin/env bash
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
# vim:set expandtab shiftwidth=2 tabstop=2:

errorCount=0

gitTree=HEAD

checkEmptyStdout()
{
  local cmd=$1
  local message=$2
  local output=$(bash -c "$cmd")
  if [ ! -z "$output" ]
  then
    echo "=============================================================================="
    echo "Error: $message:"
    echo "------------------------------------------------------------------------------"
    echo "$output"
    errorCount=$((errorCount + 1))
  fi
  return 0
}

# TODO switch to something without extra command quoting

checkEmptyStdout \
  "git ls-tree -r -t --name-only $gitTree | tr '[:upper:]' '[:lower:]' | sort | uniq --repeated" \
  "git ls-tree: path names that only differ in case"

# Check for illegal characters in file names (on Windows).
# We could extend for more that we do not want.
checkEmptyStdout \
  "git ls-tree -r -t --name-only $gitTree | grep '[\"<>|:*?\\]'" \
  "git ls-tree: path names will illegal characters encountered"

checkEmptyStdout \
  "for i in .gitattributes .gitignore ; do
    test -z \$(git ls-tree --name-only $gitTree \"\$i\") && echo \"\$i\";
  done" \
  "Critical file(s) missing"

# Note: "binary" is a built-in macro attribute, that expands to "-diff -merge -text".
checkEmptyStdout \
  "git ls-tree --full-tree -l -r $gitTree |
    grep '^[0-9]* blob ' |
    cut -f 2- |
    git check-attr text --cached --stdin |
    grep -v 'text: set' |
    cut -d: -f1 |
    git check-attr diff merge text --cached --stdin |
    grep -v 'diff: unset' |
    grep -v 'merge: unset' |
    grep -v 'text: unset' |
    cut -d: -f1 |
    sort -u" \
  "files that are neither marked as binary nor text, please extend .gitattributes"

# Note: we rely on binary files being identified correctly (cf. above)
# TODO limit to specific extensions if necessary
checkEmptyStdout \
  "git grep -I -l \$'\t' $gitTree | cut -d: -f2- | grep -v -e ^external/ -e \.sln$ -e /Makefile$ -e project\.pbxproj -e Info\.plist -e sampledata/.*\.txt -e public_samples/speech/sampledata/.*\.txt" \
  "text file(s) with hard tabs encountered"

checkEmptyStdout \
  "git ls-tree --full-tree -l -r $gitTree | grep '^100644 blob .*\\.sh'" \
  "Shell scripts should have executable permissions set, please fix"

checkEmptyStdout \
  "git grep -I -l $'\xEF\xBB\xBF' $gitTree | cut -d: -f2 | grep -v -e sampledata/.*\.txt -e public_samples/speech/sampledata/.*\.txt" \
  "BOM detected, please remove from beginning of files (or add an exception)"

# Note: we rely on binary files being identified correctly (cf. above)
checkEmptyStdout \
  "git grep -I -l $'\r$' $gitTree | cut -d: -f2" \
  "CR LF detected in the (in-repo) version of text file(s), please change to just LF"

if [ $errorCount -ne 0 ]
then
  echo "=============================================================================="
  echo FATAL: $errorCount error\(s\)
  exit 1
fi
exit 0
