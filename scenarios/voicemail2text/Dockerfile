FROM ubuntu:20.04
RUN sed -i '/jessie/d' /etc/apt/sources.list && apt-get update

# ubuntu 20.04 by default installs Python 3.8
RUN apt-get install -y ffmpeg python3.9 python3.9-distutils vim wget
RUN ln -s /usr/bin/python3.9 /usr/bin/python
#symbolic link between default python3 folder and new installed python3.9 ver
RUN rm /usr/bin/python3 && \
    ln -s /usr/bin/python3.9 /usr/bin/python3
#get pip to install requirements
RUN wget https://bootstrap.pypa.io/get-pip.py && \
    python3 get-pip.py && \
    rm get-pip.py

WORKDIR /app
COPY . /app
RUN pip3 install -r /app/requirements.txt