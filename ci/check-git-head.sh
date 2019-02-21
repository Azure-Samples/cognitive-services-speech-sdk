#!/bin/bash
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
  "for i in .gitattributes .gitignore .gitmodules; do
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
  "git grep -I -l \$'\t' $gitTree | cut -d: -f2- | grep -v ^external/ | grep -v \.sln$ | grep -v /Makefile$ | grep -v project\.pbxproj | grep -v \.plist | grep -v ci/licensefiles/" \
  "text file(s) with hard tabs encountered"

checkEmptyStdout \
  "git ls-tree --full-tree -l -r $gitTree | grep -E '^100644 blob .*(\\.sh|/gradlew|\\.expect)$'" \
  "Shell scripts should have executable permissions set, please fix"

checkEmptyStdout \
  "git grep -I -l $'\xEF\xBB\xBF' $gitTree | cut -d: -f2" \
  "BOM detected, please remove from beginning of files (or add an exception)"

# Note: we rely on binary files being identified correctly (cf. above)
checkEmptyStdout \
  "git grep -I -l $'\r$' $gitTree | cut -d: -f2" \
  "CR LF detected in the (in-repo) version of text file(s), please change to just LF"

# Note: test only lists the files, not the actual matches.
# Meant to be a cheap check, nothign water-proof (which should be client-side anyway).
checkEmptyStdout \
  "git grep -l -i -I -P '[^a-f0-9][a-f0-9]{32}[^a-f0-9]' $gitTree | fgrep -e docs/articles/usp-translation.md -e public_samples/quickstart/csharp-unity/Assets/Scenes/HelloWorld.unity -e public_samples/quickstart/csharp-unity/ProjectSettings/GraphicsSettings.asset -v | cut -d: -f2-" \
  "Potentially subscription key checked in? Double check, if necessary modify white-list in this script: git grep -i -I -P '[^a-f0-9][a-f0-9]{32}[^a-f0-9]'"

checkEmptyStdout \
  "git grep -l -i -I carbon $gitTree -- public_samples source/public source/bindings/csharp source/bindings/objective-c ThirdPartyNotices.md REDIST.txt license.md | grep -v -e CMakeLists\.txt$ -e carbon_[a-z]*\.i$ | cut -d: -f2-" \
  "Remove Carbon in files"

checkEmptyStdout \
  "git grep -I -P '///\s*<\w+>' source/bindings/java" \
  "No XMLDoc comment in Java, please change to JavaDoc format"

if [ $errorCount -ne 0 ]
then
  echo "=============================================================================="
  echo FATAL: $errorCount error\(s\)
  exit 1
fi
exit 0
