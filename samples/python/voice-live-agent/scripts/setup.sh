 #!/bin/sh

. ./scripts/load_python_env.sh

./.venv/bin/python ./scripts/setup_aisearch.py
./.venv/bin/python ./scripts/setup_cosmos.py
