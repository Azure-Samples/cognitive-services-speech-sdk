#!/usr/bin/env bash

set -e

BICEP_VERSION="v0.29.47"

# if az bicep is not installed, install it else upgrade it
if ! command -v az bicep &> /dev/null || [ "$(az bicep version --query 'bicepVersion' -o tsv)" != "$BICEP_VERSION" ]; then
    az bicep install --version "$BICEP_VERSION"
fi

TEMPLATES=()
FILES=()

for ARG in "$@"; do
    # If the argument is supplied with "-f", then it is a template file that needs to be built
    if [[ $ARG == -f=* ]]; then
        TEMPLATES+=("${ARG#-f=}")
    else
    # Otherwise, it is a file that has been edited
        az bicep format -f "$ARG" &
        FILES+=($ARG)
    fi
done

wait

git add "${FILES[@]}"

# Build the templates
for TEMPLATE in "${TEMPLATES[@]}"; do
    az bicep build -f "$TEMPLATE"
    git add "${TEMPLATE%.bicep}.json" # Change the extension from .bicep to .json
done