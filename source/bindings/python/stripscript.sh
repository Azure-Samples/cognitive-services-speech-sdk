#!/bin/bash

# strip python annotations from source file so as to not confuse sphinx
set -e
WORKDIR=$1

strip-hints "$WORKDIR"/speech_py_impl.py > speech_py_impl_stripped.py &&
mv speech_py_impl_stripped.py "$WORKDIR"/speech_py_impl.py
