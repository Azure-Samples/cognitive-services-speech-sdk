FROM mcr.microsoft.com/azurelinux/base/python:3.12 AS backend

RUN tdnf distro-sync -y && \
    tdnf install -y ca-certificates && \
    tdnf clean all

RUN pip install poetry

RUN poetry config virtualenvs.create false

WORKDIR /app

COPY pyproject.toml poetry.lock ./

RUN poetry install --no-dev --no-root

COPY realtime-api-plus /app

ENTRYPOINT [ "python3", "app.py" ]

FROM mcr.microsoft.com/azurelinux/base/nodejs:20 AS web

RUN tdnf distro-sync -y && \
    tdnf install -y jq && \
    tdnf clean all

COPY download-pkg.sh .
RUN chmod +x download-pkg.sh
RUN ./download-pkg.sh

WORKDIR /web
COPY web/package*.json ./
RUN npm install
COPY web/ .
RUN npm run build

# Replace this with your application's default port
EXPOSE 5173

ENTRYPOINT [ "npm", "run", "dev" ]