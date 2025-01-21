ARG CNV_VOICE

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

EXPOSE 8080

ENTRYPOINT [ "python3", "app.py" ]

FROM mcr.microsoft.com/azurelinux/base/nodejs:20 AS web-builder

RUN tdnf distro-sync -y && \
    tdnf install -y jq git

RUN git clone https://github.com/yulin-li/aoai-realtime-audio-sdk.git
WORKDIR /aoai-realtime-audio-sdk/javascript/standalone
RUN git checkout yulin/avatar
RUN npm install && npm run build
RUN npm pack

FROM mcr.microsoft.com/azurelinux/base/nodejs:20 AS web

ARG CNV_VOICE

ENV VITE_CNV_VOICE=$CNV_VOICE

RUN tdnf distro-sync -y && \
    tdnf install -y jq && \
    tdnf clean all

# COPY download-pkg.sh .
# RUN chmod +x download-pkg.sh
# RUN ./download-pkg.sh
COPY --from=web-builder /aoai-realtime-audio-sdk/javascript/standalone/rt-client-0.5.2.tgz /rt-client-0.5.2.tgz

WORKDIR /web
COPY web/package*.json ./
RUN npm install
COPY web/ .
RUN npm run build

# Replace this with your application's default port
EXPOSE 5173

ENTRYPOINT [ "npm", "run", "dev" ]