FROM ubuntu:20.04
RUN apt-get update -y
RUN apt-get install -y build-essential \
                        wget \
                        git \
                        vim \
                        libmysqlclient-dev

WORKDIR /app
