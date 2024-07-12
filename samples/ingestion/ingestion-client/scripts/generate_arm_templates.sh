#!/usr/bin/env bash

set -e

az bicep version 2>/dev/null || az bicep install

TEMPLATES=()
FILES=()

for ARG in $@; do
    # If the argument is supplied with "-f", then it is a template file that needs to be built
    if [[ $ARG == -f=* ]]; then
        TEMPLATES+=(${ARG#-f=})
    else
    # Otherwise, it is a file that has been edited
        az bicep format -f $ARG &
        FILES+=($ARG)
    fi
done

wait

git add ${FILES[@]}

# Build the templates
for TEMPLATE in ${TEMPLATES[@]}; do
    az bicep build -f $TEMPLATE
    git add "${TEMPLATE%.bicep}.json" # Change the extension from .bicep to .json
done