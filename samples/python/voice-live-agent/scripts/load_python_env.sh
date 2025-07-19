 #!/bin/sh

echo 'Creating Python virtual environment ".venv" from scripts/requirements.txt...'
python3 -m venv .venv

echo 'Installing dependencies from "requirements.txt" into virtual environment (in quiet mode)...'
.venv/bin/python -m pip --quiet --disable-pip-version-check install -r scripts/requirements.txt