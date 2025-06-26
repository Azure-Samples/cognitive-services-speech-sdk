FROM mcr.microsoft.com/azurelinux/base/python:3.12

RUN tdnf distro-sync -y && \
    tdnf install -y ca-certificates && \
    tdnf clean all

RUN pip --version

COPY . /app

RUN python3 -m pip install -r /app/requirements.txt --no-cache-dir

WORKDIR /app

ENTRYPOINT [ "python3", "-m", "flask", "run", "-h", "0.0.0.0", "-p", "5000" ]