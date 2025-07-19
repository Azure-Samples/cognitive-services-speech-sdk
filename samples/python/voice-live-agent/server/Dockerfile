###############################################
# Base stage (shared between build and final) #
###############################################
FROM python:3.12-slim-bookworm AS base

###############
# Build stage #
###############
FROM base AS builder

## Install and configure UV, see https://docs.astral.sh/uv/ for more information
COPY --from=ghcr.io/astral-sh/uv:latest /uv /uvx /bin/

# - Silence uv complaining about not being able to use hard links,
# - tell uv to byte-compile packages for faster application startups,
ENV UV_LINK_MODE=copy \
    UV_COMPILE_BYTECODE=1 \
    UV_PROJECT_ENVIRONMENT=/app/.venv

COPY uv.lock pyproject.toml /app/
WORKDIR /app

# Sync the project into a new environment, using the frozen lockfile
RUN uv sync --frozen --no-dev --no-install-project --no-editable --all-packages

COPY *.py *.md /app/
COPY app /app/app/
COPY static /app/static/

###############
# Final image #
###############
FROM base

RUN groupadd -r app
RUN useradd -r -d /app -g app -N app

COPY --from=builder --chown=app:app /app /app
WORKDIR /app

ENV PYTHONUNBUFFERED=1

EXPOSE 8000
ENTRYPOINT ["/app/.venv/bin/python",  "server.py"]
