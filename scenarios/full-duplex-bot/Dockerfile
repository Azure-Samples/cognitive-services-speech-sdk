FROM mcr.microsoft.com/azurelinux/base/python:3.12

RUN tdnf distro-sync -y && \
    tdnf install -y ca-certificates && \
    tdnf clean all

RUN pip install poetry
RUN poetry config virtualenvs.create false

WORKDIR /app

COPY pyproject.toml poetry.lock ./

RUN poetry install --no-dev --no-root

COPY fullduplex /app

RUN python3 cache.py

ENV FORWARDED_ALLOW_IPS="*"

ENTRYPOINT [ "uvicorn", "webapp:app", "--host", "0.0.0.0", "--port", "8080", "--proxy-headers" ]