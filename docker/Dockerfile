FROM debian:stretch
LABEL maintainer="MarteX Developers <dev@martexcoin.org>"
LABEL description="Dockerised MarteXCore, built from Travis"

RUN apt-get update && apt-get -y upgrade && apt-get clean && rm -fr /var/cache/apt/*

COPY bin/* /usr/bin/
