ARG CNV_VOICE

FROM mcr.microsoft.com/azurelinux/base/nodejs:20 AS web-builder

RUN tdnf distro-sync -y && \
    tdnf install -y jq git

RUN git clone https://github.com/yulin-li/aoai-realtime-audio-sdk.git
WORKDIR /aoai-realtime-audio-sdk/javascript/standalone
RUN git checkout feature/voice-agent
RUN git checkout 96d2089ef0829f687abf14d52ba1aaba2e8886a9
RUN npm install && npm run build
RUN npm pack

FROM mcr.microsoft.com/azurelinux/base/nodejs:20 AS web

ARG CNV_VOICE

ENV VITE_CNV_VOICE=$CNV_VOICE

RUN tdnf distro-sync -y && \
    tdnf install -y jq && \
    tdnf clean all

COPY --from=web-builder /aoai-realtime-audio-sdk/javascript/standalone/rt-client-0.5.2.tgz /web/rt-client-0.5.2.tgz

WORKDIR /web
COPY package*.json ./
RUN npm install
COPY . .
RUN npm run build

FROM mcr.microsoft.com/azurelinux/base/python:3 AS final

RUN tdnf distro-sync -y && \
    tdnf install -y ca-certificates && \
    tdnf upgrade -y && \
    tdnf clean all

COPY --from=web /web/out /web/out
COPY app.py /web/app.py

RUN --mount=type=cache,target=/root/.cache/pip pip install aiohttp azure-identity "azure-ai-agents"

WORKDIR /web

EXPOSE 3000

ENTRYPOINT [ "python3", "app.py" ]